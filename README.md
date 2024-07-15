# arduino-sg200x

  该项目是为sg200x支持Arduino而建立，可以适配绝大多数Arduino的API,  详情参考[官方文档](https://www.arduino.cc/reference/en/)。

|  平台   |  MilkV Duo  |  MilkV Duo256   | MilkV DuoS  |
|  ----  | ----  |  ----  | ----  |
|  GPIO  | 支持 | 支持  | 支持 |
|  UART  | 支持 | 支持  | 支持 |
|  I2C  | 支持 |  支持  | 支持(1) |
|  SPI  | 支持 | 支持  | 支持 |
|  PWM  | 支持 | 支持  | 支持 |
|  ADC  | 支持 |  支持  | 支持 |
|  MailBox  | 支持 |  支持  | 暂不支持(2) |

* *1: I2C-1 和 I2C-2 之间无法互相发送数据，原因暂不明确。I2C-1 和 I2C-2 任意一个和 I2C-4 收发数据都正常。*
* *2: 大核心的镜像中缺少 Mailbox 的设备。*

## 快速上手

本篇将说明如何安装 Arduino-Sg200x 支持，以及如何快速使用。

### 安装Arduino IDE

执行以下步骤：

1. Arduino IDE支持Windows，Linux， macOS三种操作系统，根据你的系统到[arduino官方网站](https://www.arduino.cc/en/software)下载对应安装包进行安装，建议使用2.3.X以上版本。

2. 启动Arduino并打开 ``文件`` > ``首选项`` 窗口。

3. 在 ``附加开发板管理器网址:`` 字段中输入package json文件路径。您可以添加多个URL，并用逗号分隔它们。

4. 从 ``工具`` > ``开发板`` 菜单中打开 ``开发板管理器`` 并安装 *SG200X* 平台。

5. 安装后从 ``工具`` > ``开发板`` 菜单中选择您的 *SG200X* 开发板。

   到此Arduino Sg200x开发环境便已经安装完成，下面就可以尝试简单使用下了。

### 运行一个示例

运行一个简单示例的流程如下：

1. 将Duo板子插入到电脑USB
2. 
   打开 ``文件`` > ``示例`` 找到Duo的例子，选择 ``01.Basics`` > ``Blink`` ，该示例演示了控制led闪烁。

3. 点击 ``上传`` 按钮，会执行编译并烧录程序到板子。
    出现上传成功提示，就可以看到板子上的led在闪烁了。到此为止就完成了示例代码的运行演示。

如果想要自己在示例代码基础上修改，可以点击 ``保存`` 按钮，把代码保存到自己的项目路径下，然后在进行修改编译上传。


### 社区库使用

Arduino社区提供了众多的第三方库，使用第三方库可以大大简化我们的开发工作量，快速实现想要的功能。使用方法如下：

1. 从 ``工具`` > ``管理库`` 菜单中打开 ``库管理器`` ，在搜索框直接搜索想要找的库，例如 ``Adafruit GFX`` 。找到后安装即可。
2. 从 ``项目`` > ``加载库`` 菜单中加载要使用的库，就会自动在代码编辑页面顶部加入该库的头文件，以便我们调用库的API。


### 编译好的二进制程序分发

如果你想把编译好程序的bin文件分享给他人使用，参考以下说明：

1. 点击 ``验证`` 或者 ``项目`` > ``验证/编译`` 按钮，确保程序可以正常编译。

2. 点击 ``项目`` > ``导出已编译的二进制文件`` 会将刚刚编好的bin文件导出到你的程序项目路径下。
    *注意：* 如果打开的是示例代码，则需要先保存项目文件到自己的项目路径下，才可以成功导出bin。
