## tools

烧录相关的工具已迁到独立仓库 [kubuds/rproc-burn](https://github.com/kubuds/rproc-burn) 维护：

- `burntool`：主机端上传工具，Arduino IDE 点「上传」时由 `platform.txt` 调用。Board Manager 安装本平台时会自动从该仓库的 Release 下载，版本号见 `package/config.json` 的 `tools[]`。
- `burnd`：板端守护进程，随 Milk-V 的 SD 卡镜像预装。
- duo-buildroot-sdk 的 remoteproc 补丁。

串口协议和固件格式约束的文档也在那边的 `docs/` 下。
