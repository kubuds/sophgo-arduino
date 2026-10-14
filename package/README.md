## package

Arduino Board Manager 发布脚本。

```bash
python3 package/release.py --tag v0.3.0 --out dist          # CI 使用
python3 package/release.py --version 0.3.0 --fresh-index    # 本地试跑，不合并旧 index
python3 package/release.py --version 0.3.0 --skip-tools     # 离线试跑，不拉 burntool 的发布信息
```

产物：

- `sophgo-arduino-<version>.zip`：平台包，只包含 `config.json` 里 `platform.include` 列出的、受 git 跟踪的文件
- `package_sg200x_index.json`：合并了上一次发布内容的索引

烧录工具 `burntool` 不在这里构建：`config.json` 里它的 `source` 是 `systems-json`，脚本按 `version` 去 [kubuds/sg200x-burn](https://github.com/kubuds/sg200x-burn) 的 Release 拉 `burntool-<version>-systems.json`，把其中的 URL 和校验和写进索引。升级 burntool 只需改 `config.json` 里的版本号。

版本号以 git tag 为准，`Version` 和 `package.json` 必须与之一致，否则脚本报错。
