#!/usr/bin/env python3
"""Build the Arduino Board Manager release of sophgo-arduino.

Produces, in the output directory:

* the platform archive (core, variants, libraries, board definitions),
* one burntool archive per supported host, cross-compiled with Go,
* the package index JSON, merged with the index of the previous release
  so older versions stay installable.

Only the Python standard library, git and go are required.

    python3 package/release.py --tag v0.3.0 --out dist
    python3 package/release.py --version 0.3.0 --fresh-index --skip-tools
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request
import zipfile
from dataclasses import dataclass
from pathlib import Path

PACKAGE_DIR = Path(__file__).resolve().parent
REPO_DIR = PACKAGE_DIR.parent
CONFIG_FILE = PACKAGE_DIR / "config.json"


class ReleaseError(Exception):
    """A condition that must stop the release with a clear message."""


@dataclass
class Artifact:
    """A file that will be attached to the GitHub release."""

    path: Path
    url: str

    @property
    def name(self) -> str:
        return self.path.name

    @property
    def checksum(self) -> str:
        return "SHA-256:" + hashlib.sha256(self.path.read_bytes()).hexdigest()

    @property
    def size(self) -> str:
        return str(self.path.stat().st_size)


# --------------------------------------------------------------------------
# configuration and version


def load_config() -> dict:
    with CONFIG_FILE.open(encoding="utf-8") as fh:
        return json.load(fh)


def resolve_version(args: argparse.Namespace, cfg: dict) -> str:
    """Pick the release version from --version or --tag and make sure the
    files that also carry a version agree with it."""
    prefix = cfg["repo"]["tagPrefix"]
    if args.version:
        version = args.version
    elif args.tag:
        if not args.tag.startswith(prefix):
            raise ReleaseError(f"tag {args.tag!r} does not start with {prefix!r}")
        version = args.tag[len(prefix):]
    else:
        version = (REPO_DIR / "Version").read_text(encoding="utf-8").strip()

    if not re.fullmatch(r"\d+\.\d+\.\d+(-[0-9A-Za-z.]+)?", version):
        raise ReleaseError(f"{version!r} is not a semantic version")

    if not args.no_version_check:
        check_version_files(version)
    return version


def check_version_files(version: str) -> None:
    """Fail when Version or package.json disagree with the release tag."""
    file_version = (REPO_DIR / "Version").read_text(encoding="utf-8").strip()
    with (REPO_DIR / "package.json").open(encoding="utf-8") as fh:
        pkg_version = json.load(fh)["version"]
    for name, found in (("Version", file_version), ("package.json", pkg_version)):
        if found != version:
            raise ReleaseError(
                f"{name} says {found!r} but releasing {version!r}; "
                "update the file or pass --no-version-check"
            )


def version_key(version: str) -> tuple:
    """Sort key for versions such as 0.2.7 or 13.2.0-2 (numeric aware)."""
    parts = re.split(r"[.\-]", version)
    return tuple((0, int(p)) if p.isdigit() else (1, p) for p in parts)


# --------------------------------------------------------------------------
# archives


def tracked_files(repo: Path) -> list[Path]:
    out = subprocess.run(
        ["git", "-C", str(repo), "ls-files", "-z"],
        check=True, capture_output=True,
    ).stdout
    return [Path(p) for p in out.decode().split("\0") if p]


def make_archive(dest: Path, root: str, files: list[tuple[str, Path]]) -> None:
    """Write files into dest (zip or tar.gz) under a single top directory.

    files holds (name inside root, source path) pairs. Arduino strips the
    single top directory when it extracts the archive.
    """
    dest.parent.mkdir(parents=True, exist_ok=True)
    if dest.name.endswith(".zip"):
        with zipfile.ZipFile(dest, "w", zipfile.ZIP_DEFLATED) as zf:
            for name, src in files:
                zf.write(src, f"{root}/{name}")
    elif dest.name.endswith(".tar.gz"):
        with tarfile.open(dest, "w:gz") as tf:
            for name, src in files:
                tf.add(src, f"{root}/{name}")
    else:
        raise ReleaseError(f"unsupported archive type: {dest.name}")


def build_platform_archive(cfg: dict, version: str, out: Path) -> Artifact:
    """Pack the tracked files listed in platform.include."""
    plat = cfg["platform"]
    include = set(plat["include"])
    files = []
    for rel in tracked_files(REPO_DIR):
        if rel.parts[0] in include:
            files.append((rel.as_posix(), REPO_DIR / rel))
    if not files:
        raise ReleaseError("platform archive would be empty; check platform.include")

    name = plat["archiveFileName"].format(version=version)
    dest = out / name
    make_archive(dest, cfg["repo"]["name"], files)
    log(f"platform archive: {dest} ({len(files)} files)")
    return Artifact(dest, download_url(cfg, version, name))


def build_go_tool(cfg: dict, tool: dict, version: str, out: Path) -> list[dict]:
    """Cross-compile a Go tool for every host and archive each binary."""
    go = shutil.which("go")
    if go is None:
        raise ReleaseError("go not found in PATH; install Go or pass --skip-tools")
    src = REPO_DIR / tool["dir"]
    tool_version = tool["version"]
    systems = []
    with tempfile.TemporaryDirectory() as tmp:
        for host, target in tool["hosts"].items():
            goos, goarch = target.split("/")
            exe = tool["name"] + (".exe" if goos == "windows" else "")
            binary = Path(tmp) / host / exe
            binary.parent.mkdir(parents=True)
            env = dict(os.environ, GOOS=goos, GOARCH=goarch, CGO_ENABLED="0")
            subprocess.run(
                [go, "build", "-trimpath",
                 "-ldflags", f"-s -w -X main.version={tool_version}",
                 "-o", str(binary), "."],
                cwd=src, env=env, check=True,
            )
            ext = "zip" if goos == "windows" else "tar.gz"
            name = tool["archiveFileName"].format(version=tool_version, host=host, ext=ext)
            dest = out / name
            make_archive(dest, tool["name"], [(exe, binary)])
            art = Artifact(dest, download_url(cfg, version, name))
            log(f"tool archive: {dest}")
            systems.append({
                "host": host,
                "url": art.url,
                "archiveFileName": art.name,
                "checksum": art.checksum,
                "size": art.size,
            })
    return systems


def download_url(cfg: dict, version: str, file: str) -> str:
    repo = cfg["repo"]
    return repo["downloadUrl"].format(
        owner=repo["owner"], name=repo["name"],
        tag=repo["tagPrefix"] + version, file=file,
    )


# --------------------------------------------------------------------------
# package index


def fetch_previous_index(cfg: dict) -> dict | None:
    """Download the index attached to the newest GitHub release, if any."""
    repo = cfg["repo"]
    api = f"https://api.github.com/repos/{repo['owner']}/{repo['name']}/releases?per_page=10"
    headers = {"Accept": "application/vnd.github+json"}
    token = os.environ.get("GITHUB_TOKEN")
    if token:
        headers["Authorization"] = f"Bearer {token}"
    try:
        with urllib.request.urlopen(urllib.request.Request(api, headers=headers)) as resp:
            releases = json.load(resp)
    except Exception as exc:  # network or API failure: report and start fresh
        log(f"cannot list releases ({exc}); generating a fresh index")
        return None

    for rel in releases:
        for asset in rel.get("assets", []):
            if asset["name"] == repo["indexFileName"]:
                log(f"previous index from release {rel['tag_name']}")
                with urllib.request.urlopen(asset["browser_download_url"]) as resp:
                    return json.load(resp)
    log("no previous index found; generating a fresh index")
    return None


def platform_entry(cfg: dict, version: str, archive: Artifact) -> dict:
    plat = cfg["platform"]
    return {
        "name": plat["name"],
        "architecture": plat["architecture"],
        "version": version,
        "category": plat["category"],
        "url": archive.url,
        "archiveFileName": archive.name,
        "checksum": archive.checksum,
        "size": archive.size,
        "boards": plat["boards"],
        "help": plat["help"],
        "toolsDependencies": [
            {"packager": t["packager"], "name": t["name"], "version": t["version"]}
            for t in cfg["tools"]
        ],
    }


def tool_entry(tool: dict, systems: list[dict]) -> dict:
    return {"name": tool["name"], "version": tool["version"], "systems": systems}


def merge_index(cfg: dict, previous: dict | None, platform: dict,
                tools: list[dict], force: bool) -> dict:
    """Insert the new platform and tool releases into the index.

    Platforms are listed newest first. A platform version that already
    exists is an error unless force is set, in which case it is replaced.
    Tool releases are keyed by (name, version) and replaced when present.
    """
    pkg_cfg = cfg["package"]
    index = previous or {"packages": []}
    package = next((p for p in index["packages"] if p["name"] == pkg_cfg["name"]), None)
    if package is None:
        package = dict(pkg_cfg, platforms=[], tools=[])
        index["packages"].insert(0, package)
    else:
        package.update(pkg_cfg)

    platforms = [p for p in package["platforms"] if p["version"] != platform["version"]]
    if len(platforms) != len(package["platforms"]) and not force:
        raise ReleaseError(
            f"platform {platform['version']} already exists in the index; use --force to replace it"
        )
    platforms.append(platform)
    platforms.sort(key=lambda p: version_key(p["version"]), reverse=True)
    package["platforms"] = platforms

    for new in tools:
        package["tools"] = [
            t for t in package["tools"]
            if (t["name"], t["version"]) != (new["name"], new["version"])
        ]
        package["tools"].append(new)
    package["tools"].sort(key=lambda t: (t["name"], version_key(t["version"])), reverse=True)
    return index


# --------------------------------------------------------------------------
# entry point


def log(msg: str) -> None:
    print(">>>", msg, file=sys.stderr)


def github_output(**values: str) -> None:
    """Expose values to later workflow steps when running in GitHub Actions."""
    path = os.environ.get("GITHUB_OUTPUT")
    if not path:
        return
    with open(path, "a", encoding="utf-8") as fh:
        for key, value in values.items():
            fh.write(f"{key}={value}\n")


def parse_args(argv: list[str]) -> argparse.Namespace:
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ver = ap.add_mutually_exclusive_group()
    ver.add_argument("--tag", help="git tag being released, e.g. v0.3.0")
    ver.add_argument("--version", help="release version without the tag prefix")
    ap.add_argument("--out", type=Path, default=REPO_DIR / "dist",
                    help="output directory (default: dist)")
    ap.add_argument("--fresh-index", action="store_true",
                    help="do not merge with the previously published index")
    ap.add_argument("--force", action="store_true",
                    help="replace an already published platform version")
    ap.add_argument("--skip-tools", action="store_true",
                    help="do not build Go tools (index will lack their systems)")
    ap.add_argument("--no-version-check", action="store_true",
                    help="do not require Version and package.json to match")
    return ap.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    cfg = load_config()
    version = resolve_version(args, cfg)
    out: Path = args.out
    out.mkdir(parents=True, exist_ok=True)
    log(f"releasing {cfg['repo']['name']} {version} into {out}")

    archive = build_platform_archive(cfg, version, out)

    tools = []
    for tool in cfg["tools"]:
        if tool["source"] == "external":
            tools.append(tool_entry(tool, tool["systems"]))
        elif tool["source"] == "go":
            if args.skip_tools:
                log(f"skipping build of {tool['name']}")
                continue
            tools.append(tool_entry(tool, build_go_tool(cfg, tool, version, out)))
        else:
            raise ReleaseError(f"unknown tool source {tool['source']!r} for {tool['name']}")

    previous = None if args.fresh_index else fetch_previous_index(cfg)
    index = merge_index(cfg, previous, platform_entry(cfg, version, archive), tools, args.force)

    index_file = out / cfg["repo"]["indexFileName"]
    index_file.write_text(json.dumps(index, indent=2) + "\n", encoding="utf-8")
    log(f"index: {index_file}")

    github_output(
        version=version,
        index_json_file_name=index_file.name,
        index_json_status="FRESH" if previous is None else "UPDATED",
    )
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main(sys.argv[1:]))
    except ReleaseError as exc:
        print("error:", exc, file=sys.stderr)
        sys.exit(1)
    except subprocess.CalledProcessError as exc:
        print("error: command failed:", " ".join(map(str, exc.cmd)), file=sys.stderr)
        sys.exit(1)
