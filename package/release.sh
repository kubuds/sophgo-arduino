#!/usr/bin/env bash
#

set -e

get_file_size(){
	local file="$1"
	if stat --version >/dev/null 2>&1; then
		# GNU version
		stat --printf="%s" "$file"
	else
		# BSD version
		stat -f "%z" "$file" || \
			fatal "Unknown stat version"
	fi
}

_do() {
	echo -e "\x1b[1m\x1b[32m>>>\x1b[0m" "$@" >&2
	"$@"
}

warn() {
	echo "$@" >&2
}
fatal() {
	local ret=1
	if [[ $1 =~ ^[[:digit:]]{1,3}$ ]] && (( $1 <= 255 )); then
		ret="$1"
		shift
	fi
	warn "$@"
	exit "$ret"
}

gh_latest_two_tag() {
	local owner=$1 name=$2
	local res query query_f
	local -a tags
	printf -v query \
		'{ "query": "query { repository(owner: \\\"%s\\\", name: \\\"%s\\\") { refs(refPrefix: \\\"refs/tags/\\\",orderBy: { field: TAG_COMMIT_DATE, direction: DESC }, first: 2) { edges { node { name } } } } }" }' \
		"${owner}" "${name}"
	res=$(_do curl -s -X POST \
		-H "Accept: application/json" \
		-H "Authorization: Bearer ${GITHUB_TOKEN}" \
		-d "${query}" https://api.github.com/graphql ) || return $?
	tags=$(_do echo "$res" | _do jq --raw-output '.data.repository.refs.edges[].node.name')
	echo -n "${tags[@]}"
}

CURDIR="$(dirname "$(realpath "$0")")"
REPODIR=$(realpath "${CURDIR}/../")
: "${WORKDIR:="$(realpath "${REPODIR}/../")"}"

if [[ "$1" == "--ignore-previous-versions" ]]; then
	GENERATE_FRESH_INDEX_JSON_FILE=1
fi

. "${CURDIR}/_vercmp.sh"

if [[ $(echo "$BASH_VERSION" | cut -d'.' -f1) -lt 4 ]]; then
	fatal "The bash version is too old, it's better to upgrade to 5 or newer."
fi

CONFIG_JSON_DATA=$(cat "${CURDIR}/config.json")
jq_value() {
	jq --raw-output "$1" <<<"$CONFIG_JSON_DATA"
}

ARDUINO_VERSION="$(jq_value '.current .version')"
INDEX_JSON_FILE_NAME="$(jq_value '.basicInfo .indexJsonFileName')"
REPO_OWNER="$(jq_value '.basicInfo .repoOwner')"
REPO_NAME="$(jq_value '.basicInfo .repoName')"

declare -A FILENAME CORRESPONDINGPARENTDIR CORRESPONDINGDIRNAME EXCLUDEPATTERN SHASUM SIZEINBYTES

FILELIST=( "arduino-sophgo" )

FILENAME[arduino-sophgo]="arduino-sophgo.zip"
TOOLNAME[arduino-sophgo]="arduino-sophgo"
CORRESPONDINGPARENTDIR[arduino-sophgo]="${WORKDIR}"
CORRESPONDINGDIRNAME[arduino-sophgo]="$(basename "$REPODIR")"
EXCLUDEPATTERN[arduino-sophgo]="arduino-sophgo/.git**;arduino-sophgo/tools/*"
# EXCLUDEPATTERN relative to the CORRESPONDINGPARENTDIR

# add internal tools which need to be packaged
append_filelist() {
	local json_data tool_name tool_dir exclude_patterns key _key exists filename
	while read -r json_data; do
		tool_name=$(jq --raw-output '.name' <<<"$json_data")
		tool_dir=$(jq --raw-output '.dir' <<<"$json_data")
		exclude_patterns=$(jq --raw-output '.excludePatterns' <<<"$json_data")
		if [[ ${tool_dir} == null ]]; then
			warn "invalid internal tool configuration, missing \"dir\""
			warn "ignore '${tool_name}', data: '$json_data'"
			continue
		fi
		while read -r filename; do
			key="${filename//./-}"
			exists=0
			for _key in "${FILELIST[@]}"; do
				if [[ $_key == "$key" ]]; then
					exists=1
					break
				fi
			done
			if [[ $exists == 0 ]]; then
				FILELIST+=( "$key" )
				FILENAME[$key]="$filename"
				TOOLNAME[$key]="$tool_name"
				CORRESPONDINGPARENTDIR[$key]="${REPODIR}/$(dirname "${tool_dir}")"
				CORRESPONDINGDIRNAME[$key]="$(basename "${tool_dir}")"
				EXCLUDEPATTERN[$key]="${exclude_patterns}"
			fi
		done < <(jq --raw-output ".systems[] .archiveFileName" <<<"$json_data")
	done < <(jq -c '.current .toolsDependencies[] | select(.source == "internal") | { name: .name, dir: .dir, excludePatterns: .excludePatterns, systems: .systems }' <<< "$CONFIG_JSON_DATA")
}
append_filelist

echo "Creating Release Files ..."

pushd "$WORKDIR" >/dev/null || fatal "pushd to '$WORKDIR' failed"
trap '
popd >/dev/null
' EXIT

create_package() {
	local file_index

	for file_index in "${FILELIST[@]}"; do
		echo ":: handling item '$file_index'"

		local filename the_parent_dir the_dir exclude_patterns _exclude_pattern _exclude_arg_name
		local -a _extra_args=() _compress_args=()

		eval "filename=\"\${FILENAME[${file_index}]}\""
		eval "the_parent_dir=\"\${CORRESPONDINGPARENTDIR[${file_index}]}\""
		eval "the_dir=\"\${CORRESPONDINGDIRNAME[${file_index}]}\""
		eval "exclude_patterns=\"\${EXCLUDEPATTERN[${file_index}]}\""

		case "$filename" in
			*.zip)
				_exclude_arg_name="-x"
				_compress_args=("zip" "-qr")
				;;
			*.tar.gz)
				_exclude_arg_name="--exclude"
				_compress_args=("tar" "-zcf")
				;;
			*)
				fatal "unknown filename extension."
				;;
		esac

		# the real patterns in the EXCLUDEPATTERN element are separated by ';'
		local IFS=";"
		set -f # disable the pathname expansion here
		for _exclude_pattern in $exclude_patterns; do
			_extra_args+=( "$_exclude_arg_name" "$_exclude_pattern" )
		done
		set +f # re-enable it

		# create the compressed pkg
		_do rm -rf "${filename}"
		pushd "$the_parent_dir" >/dev/null || fatal "pushd to '$the_parent_dir' failed"
		_do "${_compress_args[@]}" "${WORKDIR}/${filename}" "$the_dir" "${_extra_args[@]}"
		popd >/dev/null || fatal "popd failed"

		# get the shasum and filesize
		SHASUM[$file_index]="$(sha256sum "$filename" | cut -d' ' -f1)"
		SIZEINBYTES[$file_index]="$(get_file_size "$filename")"
	done
}

create_package

##
# handle json
INDEX_JSON_FILE="${WORKDIR}/${INDEX_JSON_FILE_NAME}"
INDEX_JSON_FILE_TMPL="${CURDIR}/${INDEX_JSON_FILE_NAME%.json}.template.json"

if [[ -z $GENERATE_FRESH_INDEX_JSON_FILE ]]; then
	THE_LATEST_TWO_TAGS=( $(gh_latest_two_tag "$REPO_OWNER" "$REPO_NAME") )
	for __latest_tag in "${THE_LATEST_TWO_TAGS[@]}"; do
		INDEX_JSON_FILE_URL=$(
			curl -H "Authorization: Token ${GITHUB_TOKEN}" \
				"https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/releases/tags/${__latest_tag}" \
				| jq --raw-output ".assets[] | select(.name == \"${INDEX_JSON_FILE_NAME}\") | .url"
		) && break || true
	done
fi
if [[ $INDEX_JSON_FILE_URL == "" ]]; then
	GENERATE_FRESH_INDEX_JSON_FILE=1
fi

DOWNLOAD_URL_FMT="$(jq_value '.basicInfo .downloadUrlFmt')"
VERSION_PREFIX="$(jq_value '.basicInfo .versionPrefix')"
TOOL_DEPS_JSON=$(jq -c '.current .toolsDependencies | map({packager, name, version, source})' <<<"$CONFIG_JSON_DATA")

generate_url() {
	local url="$DOWNLOAD_URL_FMT"
	url=${url//@REPOOWNER@/${REPO_OWNER}}
	url=${url//@REPONAME@/${REPO_NAME}}
	url=${url//@VERSION@/${VERSION_PREFIX}${ARDUINO_VERSION}}
	url=${url//@ARCHIVENAME@/${1}}
	echo -n "$url"
}

generate_current_platform_json() {
	local json_data archive_name tool_deps
	json_data="$(jq -c '.current | del(.toolsDependencies)' <<<"$CONFIG_JSON_DATA")"
	archive_name="$(jq --raw-output '.archiveFileName' <<<"$json_data")"
	json_data=$(jq -c ". + {\"url\": \"$(generate_url "$archive_name")\"}" <<<"$json_data")
	json_data=$(jq -c ". + {\"checksum\": \"SHA-256:${SHASUM[arduino-sophgo]}\"}" <<<"$json_data")
	json_data=$(jq -c ". + {\"size\": \"${SIZEINBYTES[arduino-sophgo]}\"}" <<<"$json_data")
	tool_deps=$(jq -c '. |map({packager: .packager, name: .name, version: .version})' <<<"$TOOL_DEPS_JSON")
	jq -c ". + {\"toolsDependencies\": ${tool_deps}}" <<<"$json_data"
}

generate_tools_item_json() {
	local _name="$1" _version="$2" _source="$3" _systems_json archive_name archive_key _archive_name_did
	local -a archive_name_did
	_systems_json="$(jq -c ".current .toolsDependencies[] | select(.name == \"${_name}\") .systems" <<<"$CONFIG_JSON_DATA")"
	if [[ $_source == "internal" ]]; then
		while read -r archive_name; do
			for _archive_name_did in "${archive_name_did[@]}"; do
				if [[ $_archive_name_did == "$archive_name" ]]; then
					continue
				fi
			done
			archive_key="${archive_name//./-}"
			_systems_json="$(jq -c "(.[] | select(.archiveFileName == \"${archive_name}\")).url = \"$(generate_url "$archive_name")\"" <<<"$_systems_json")"
			_systems_json="$(jq -c "(.[] | select(.archiveFileName == \"${archive_name}\")).checksum = \"SHA-256:${SHASUM[$archive_key]}\"" <<<"$_systems_json")"
			_systems_json="$(jq -c "(.[] | select(.archiveFileName == \"${archive_name}\")).size = \"${SIZEINBYTES[$archive_key]}\"" <<<"$_systems_json")"
			archive_name_did+=( "$archive_name" )
		done < <(jq --raw-output '.[] .archiveFileName' <<<"$_systems_json")
	fi
	jq -c "{\"name\": \"${_name}\", \"version\": \"${_version}\", \"systems\": ${_systems_json} }" <<<"{}"
}

if [[ -n $GENERATE_FRESH_INDEX_JSON_FILE ]]; then
	generate_fresh() {
		local platform_json __json
		platform_json="$(generate_current_platform_json)"
		__json="$(jq -c ".packages[0] .platforms |= [ ${platform_json} ] + ." "$INDEX_JSON_FILE_TMPL")"

		# append all tools deps
		local tool_name tool_version tool_source tool_item
		while read -r tool_name tool_version tool_source; do
			tool_item=$(generate_tools_item_json "$tool_name" "$tool_version" "$tool_source")
			__json="$(jq -c ".packages[0] .tools |= [ ${tool_item} ] + ." <<< "$__json")"
		done < <(jq --raw-output '.[] | "\(.name) \(.version) \(.source)"' <<<"$TOOL_DEPS_JSON")
		jq . <<< "$__json" >"$INDEX_JSON_FILE"
	}
	generate_fresh
	echo
	echo ":::STATUS:FRESH_JSON"
else
	ARDUINO_VERSION_UPDATED=0
	TOOLS_DEPS_UPDATED=0
	get_the_latest_index_json_file() {
		curl -H "Authorization: Token ${GITHUB_TOKEN}" \
			-H "Accept: application/octet-stream" \
			-Lfo "$INDEX_JSON_FILE" "${INDEX_JSON_FILE_URL}"
	}
	get_the_latest_index_json_file

	UPDATED_JSON="$(cat "$INDEX_JSON_FILE")"
	ARDUINO_VERSION_REMOTE=$(jq --raw-output '.packages[0] .platforms[0] .version' "$INDEX_JSON_FILE")
	# .platforms[0] should be always the latest arduino-sophgo info element
	if _vercmp g "$ARDUINO_VERSION" "$ARDUINO_VERSION_REMOTE"; then
		new_platform_item() {
			local platform_json
			platform_json="$(generate_current_platform_json)"
			UPDATED_JSON="$(jq -c ".packages[0] .platforms |= [ ${platform_json} ] + ." <<<"$UPDATED_JSON")"
		}
		new_platform_item
		ARDUINO_VERSION_UPDATED=1
	else
		echo "arduino-sophgo: no new version, skip updating the corresponding array of the index json file"
	fi

	# update tools
	update_tools_deps() {
		local _tool_key _tool_name _tool_source _tool_item_remote _tool_item_new _tool_version_local _tool_version_remote=0
		for _tool in "${FILELIST[@]}"; do
			if [[ $_tool_key == "arduino-sophgo" ]]; then
				continue
			fi
			_tool_name="${TOOLNAME[$_tool_key]}"
			_tool_item_remote=$(jq -c ".packages[0] .tools[] | select(.name == \"${_tool_name}\")" <<< "$UPDATED_JSON")
			if [[ -n $_tool_item_remote ]]; then
				_tool_version_remote=$(jq --raw-output '.version' <<< "$_tool_item_remote")
			fi
			_tool_version_local=$(jq --raw-output ".current .toolsDependencies[] | select(.name == \"${_tool_name}\") .version" <<< "$CONFIG_JSON_DATA")
			if _vercmp g "$_tool_version_local" "$_tool_version_remote"; then
				_tool_source=$(jq --raw-output ".current .toolsDependencies[] | select(.name == \"${_tool_name}\") .source" <<< "$CONFIG_JSON_DATA")
				_tool_item_new=$(generate_tools_item_json "$_tool_name" "$_tool_version_local" "$_tool_source")
				UPDATED_JSON="$(jq -c ".packages[0] .tools |= [ ${_tool_item_new} ] + ." <<< "$UPDATED_JSON")"
				TOOLS_DEPS_UPDATED=1
			fi
		done
	}
	update_tools_deps

	echo
	if [[ $ARDUINO_VERSION_UPDATED != 0 ]] || [[ $TOOLS_DEPS_UPDATED != 0 ]]; then
		jq '.' <<<"$UPDATED_JSON" >"$INDEX_JSON_FILE"
		echo ":::STATUS:UPDATED_JSON:${ARDUINO_VERSION_UPDATED}:${TOOLS_DEPS_UPDATED}"
	else
		echo ":::STATUS:NO_UPDATED"
	fi
fi

# vim:sw=8:ts=8:noexpandtab
