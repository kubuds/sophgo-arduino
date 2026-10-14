## variants

每块板子一个目录，只放板级内容；芯片级内容在 `cores/sg200x/soc/` 和 `cores/sg200x/include/soc/`。

| 层 | 位置 | 内容 |
|---|---|---|
| 家族 | `cores/sg200x/include/soc/sg200x_common.h` | SG200X 家族共用的寄存器基址、中断号 |
| 芯片 | `cores/sg200x/include/soc/<soc>/soc_pins.h` | `pin_name_t`（pad 名）、`pin_func_t`（复用功能）枚举 |
| 芯片 | `cores/sg200x/soc/<soc>/<soc>_pin_maps.c` | `csi_pin_reg_offset[]`、`csi_gpio_pinmap[]` 寄存器偏移表，用 `#ifdef SOC_<SOC>` 包住 |
| 板子 | `variants/<board>/variant.h` | 选择 SoC（`#define SOC_<SOC>`），外设数量、调试串口等宏 |
| 板子 | `variants/<board>/variant_pins.h` | 排针编号到 pad 的映射，`LED_BUILTIN`、`A0`、`SDA` 等 Arduino 常量 |
| 板子 | `variants/<board>/variant_pin_maps.c` | `gpio_map[]`、`pwm_map[]`、`uart_*_map[]`、`out_pin_map[]` 等板级表 |
| 板子 | `variants/<board>/link.ld` | 小核固件的加载地址和大小，需与 Linux 镜像里 remoteproc 预留的内存一致 |

现有板子与芯片的对应：

| board | SoC |
|---|---|
| duo | cv1800b |
| duo256 | sg2002 |
| duos | sg2000 |
| duomodule01 | sg2000（Duo Module 01 EVB，与 DuoS 同镜像、同内存布局；未经实物验证） |

### 新增一块同芯片的板子

1. 复制同芯片板子的 `variants/<board>/` 目录，改名。
2. 按原理图改 `variant_pins.h` 的引脚编号和 `variant_pin_maps.c` 的板级表；`OUT_PIN_NUM` 与 `out_pin_map[]` 的长度保持一致。
3. 改 `link.ld` 的 `MEMORY` 区域，与 `boards.txt` 里的 `build.start_addr`、`build.image_size` 一致。
4. 在 `boards.txt` 加一段条目，`build.variant` 指向新目录。

### 新增一颗芯片

1. 在 `cores/sg200x/include/soc/<soc>/soc_pins.h` 放 pad 名和复用功能枚举。
2. 在 `cores/sg200x/soc/<soc>/<soc>_pin_maps.c` 放寄存器偏移表，整个文件用 `#ifdef SOC_<SOC>` 包住。文件名必须全局唯一：Arduino 把核心对象按文件基名归档，重名会互相覆盖。
3. 在板子的 `variant.h` 里 `#define SOC_<SOC> 1` 并包含对应头文件。
