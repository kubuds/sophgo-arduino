## burntool

主机端烧录工具，Arduino IDE 点击「上传」时由 `platform.txt` 调用。它通过板子的 USB 串口和 SD 卡镜像里的 `burnd` 守护进程通信（源码见 `tools/burn`），把编译好的 ELF 发到板上并重启小核。

纯 Go 实现，无 cgo，发布时交叉编译为单文件二进制，用户无需安装 Python 或其他依赖。

### 用法

```bash
burntool -p /dev/ttyACM0 -f sketch.elf        # Linux / macOS
burntool -p COM3 -f sketch.elf                # Windows
burntool -v -p /dev/ttyACM0 -f sketch.elf     # 打印每一帧，排查问题用
burntool --version
```

退出码：0 成功，1 烧录失败，2 参数错误。

### 协议

8 字节控制帧，小端：

```
command : AA 55 | cmd u16 | length u32
reply   : AA 55 | cmd u16 | status u16 | errno u16
tail    : crc32 u32 | FF FF FF FF
```

上传流程：`NONE` → `CONNECT` → `STOP` → `UPDATE(size)` → 固件原始字节 → tail → 等待校验应答 → `START`。细节见 `protocol.go` 和 `client.go`。

### 开发

```bash
go test ./...                       # 用内存管道模拟 burnd 跑协议测试
go build -o burntool .              # 本机构建
GOOS=windows GOARCH=arm64 go build  # 任意目标交叉编译
```

全部 host 的构建与打包由 `package/release.py` 完成，目标列表在 `package/config.json` 的 `tools[].hosts`。
