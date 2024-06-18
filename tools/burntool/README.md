## BurnTool

当前版本V0.3

## 运行方法
python burntool.py -p /dev/ttyUSB0 -f xxx.elf

```

usage: burntool [-h] [--chip CHIP] [--port PORT] [--baud BAUD] [--firmware FIRMWARE]

burntool.py v0.3 - Burn Firmware Tool

options:
  -h, --help            show this help message and exit
  --chip CHIP, -c CHIP  Target chip type
  --port PORT, -p PORT  Serial port device
  --baud BAUD, -b BAUD  Serial port baud rate used when flashing/reading
  --firmware FIRMWARE, -f FIRMWARE
                        Firmware Files

```