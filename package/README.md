## package

Arduino Board Manager 发布脚本。

```bash
python3 package/release.py --tag v0.3.0 --out dist          # CI 使用
python3 package/release.py --version 0.3.0 --fresh-index    # 本地试跑，不合并旧 index
python3 package/release.py --version 0.3.0 --skip-tools     # 没装 Go 时只打平台包
```

产物：

- `sophgo-arduino-<version>.zip`：平台包，只包含 `config.json` 里 `platform.include` 列出的、受 git 跟踪的文件
- `burntool-<version>-<host>.zip|tar.gz`：每个 host 一个烧录工具二进制，由 Go 交叉编译
- `package_sg200x_index.json`：合并了上一次发布内容的索引

版本号以 git tag 为准，`Version` 和 `package.json` 必须与之一致，否则脚本报错。
