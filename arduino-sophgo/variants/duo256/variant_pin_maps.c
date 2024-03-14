/*
 * Copyright (C) Cvitek Co., Ltd. 2019-2020. All rights reserved.
 */

#include "variant.h"
#include "csi_pin.h"

const pin_reg_offset_t csi_pin_reg_offset[] = {
    {SD0_CLK, 0XA00, 0X01C},
    {SD0_CMD, 0XA04, 0X020},
    {SD0_D0, 0XA08, 0X024},
    {SD0_D1, 0XA0C, 0X028},
    {SD0_D2, 0XA10, 0X02C},
    {SD0_D3, 0XA14, 0X030},
    {SD0_CD, 0X900, 0X034},
    {SD0_PWR_EN, 0X904, 0X038},
    {SPK_EN, 0X908, 0X03C},
    {UART0_TX, 0X90C, 0X040},
    {UART0_RX, 0X910, 0X044},
    {EMMC_DAT2, 0X914, 0X02c},
    {EMMC_CLK, 0X918, 0X030},
    {EMMC_DAT0, 0X91c, 0X034},
    {EMMC_DAT3, 0X920, 0X038},
    {EMMC_CMD, 0X924, 0X03c},
    {EMMC_DAT1, 0X928, 0X040},
    {JTAG_CPU_TMS, 0X930, 0X064},
    {JTAG_CPU_TCK, 0X934, 0X068},
    {IIC0_SCL, 0X93C, 0X070},
    {IIC0_SDA, 0X940, 0X074},
    {AUX0, 0X944, 0X078},
    {PWR_VBAT_DET, 0X000, 0X07C},
    {PWR_RSTN, 0X004, 0X080},
    {PWR_SEQ1, 0X008, 0X084},
    {PWR_SEQ2, 0X00C, 0X088},
    {PWR_WAKEUP0, 0X018, 0X090},
    {PWR_BUTTON1, 0X020, 0X098},
    {XTAL_XIN, 0X028, 0X0A0},
    {PWR_GPIO0, 0X02C, 0X0A4},
    {PWR_GPIO1, 0X030, 0X0A8},
    {PWR_GPIO2, 0X034, 0X0AC},
    {SD1_D3, 0X058, 0X0D0},
    {SD1_D2, 0X05C, 0X0D4},
    {SD1_D1, 0X060, 0X0D8},
    {SD1_D0, 0X064, 0X0DC},
    {SD1_CMD, 0X068, 0X0E0},
    {SD1_CLK, 0X06C, 0X0E4},
    {PWM0_BUCK, 0X804, 0X0EC},
    {ADC1, 0X810, 0X0F8},
    {USB_VBUS_DET, 0X820, 0X108},
    {PAD_ETH_TXP, 0XFFFF, 0X124},
    {PAD_ETH_TXM, 0XFFFF, 0X128},
    {PAD_ETH_RXP, 0XFFFF, 0X12C},
    {PAD_ETH_RXM, 0XFFFF, 0X130},
    {PAD_MIPIRX4N, 0XC38, 0X16C},
    {PAD_MIPIRX4P, 0XC3C, 0X170},
    {PAD_MIPIRX3N, 0XC40, 0X174},
    {PAD_MIPIRX3P, 0XC44, 0X178},
    {PAD_MIPIRX2N, 0XC48, 0X17C},
    {PAD_MIPIRX2P, 0XC4C, 0X180},
    {PAD_MIPIRX1N, 0XC50, 0X184},
    {PAD_MIPIRX1P, 0XC54, 0X188},
    {PAD_MIPIRX0N, 0XC58, 0X18C},
    {PAD_MIPIRX0P, 0XC5C, 0X190},
    {PAD_MIPI_TXM2, 0XC70, 0X1A4},
    {PAD_MIPI_TXP2, 0XC74, 0X1A8},
    {PAD_MIPI_TXM1, 0XC78, 0X1AC},
    {PAD_MIPI_TXP1, 0XC7C, 0X1B0},
    {PAD_MIPI_TXM0, 0XC80, 0X1B4},
    {PAD_MIPI_TXP0, 0XC84, 0X1B8},
    {PAD_AUD_AINL_MIC, 0XFFFF, 0X1BC},
    {PAD_AUD_AOUTR, 0XFFFF, 0X1C8},
    {GPIO_RTX, 0XC8C, 0X1CC},
    {GPIO_ZQ, 0X0E0, 0X1D0},
    {PKG_TYPE0, 0X81C, 0X104},
    {PKG_TYPE1, 0X824, 0X10C},
    {PKG_TYPE2, 0X828, 0X110},
    {MUX_SPI1_MISO, 0XFFFF, 0X114},
    {MUX_SPI1_MOSI, 0XFFFF, 0X118},
    {MUX_SPI1_CS, 0XFFFF, 0X11C},
    {MUX_SPI1_SCK, 0XFFFF, 0X120},
    {0xffffffffU, 0xffff, 0xffff}
};

const csi_pinmap_t csi_gpio_pinmap[] = {
    {SD0_CLK, 0, 7, PIN_FUNC_GPIO},
    {SD0_CMD, 0, 8, PIN_FUNC_GPIO},
    {SD0_D0, 0, 9, PIN_FUNC_GPIO},
    {SD0_D1, 0, 10, PIN_FUNC_GPIO},
    {SD0_D2, 0, 11, PIN_FUNC_GPIO},
    {SD0_D3, 0, 12, PIN_FUNC_GPIO},
    {SD0_CD, 0, 13, PIN_FUNC_GPIO},
    {SD0_PWR_EN, 0, 14, PIN_FUNC_GPIO},
    {SPK_EN, 0, 15, PIN_FUNC_GPIO},
    {UART0_TX, 0, 16, PIN_FUNC_GPIO},
    {UART0_RX, 0, 17, PIN_FUNC_GPIO},
    {JTAG_CPU_TCK, 0, 18, PIN_FUNC_GPIO},
    {JTAG_CPU_TMS, 0, 19, PIN_FUNC_GPIO},
    {EMMC_DAT2, 0, 26, PIN_FUNC_GPIO},
    {EMMC_CLK, 0, 22, PIN_FUNC_GPIO},
    {EMMC_DAT0, 0, 25, PIN_FUNC_GPIO},
    {EMMC_DAT3, 0, 27, PIN_FUNC_GPIO},
    {EMMC_CMD, 0, 23, PIN_FUNC_GPIO},
    {EMMC_DAT1, 0, 24, PIN_FUNC_GPIO},
    {JTAG_CPU_TMS, 0, 19, PIN_FUNC_GPIO},
    {JTAG_CPU_TCK, 0, 18, PIN_FUNC_GPIO},
    {IIC0_SCL, 0, 28, PIN_FUNC_GPIO},
    {IIC0_SDA, 0, 29, PIN_FUNC_GPIO},
    {AUX0, 0, 30, PIN_FUNC_GPIO},
    {PWM0_BUCK, 1, 0, PIN_FUNC_GPIO},
    {ADC1, 1, 3, PIN_FUNC_GPIO},
    {USB_VBUS_DET, 1, 6, PIN_FUNC_GPIO},
    {GPIO_RTX, 1, 23, PIN_FUNC_GPIO},
    {PAD_ETH_TXP, 1, 25, PIN_FUNC_GPIO},
    {PAD_ETH_TXM, 1, 24, PIN_FUNC_GPIO},
    {PAD_ETH_RXP, 1, 27, PIN_FUNC_GPIO},
    {PAD_ETH_RXM, 1, 26, PIN_FUNC_GPIO},
    {PAD_MIPIRX4N, 2, 2, PIN_FUNC_GPIO},
    {PAD_MIPIRX4P, 2, 3, PIN_FUNC_GPIO},
    {PAD_MIPIRX3N, 2, 4, PIN_FUNC_GPIO},
    {PAD_MIPIRX3P, 2, 5, PIN_FUNC_GPIO},
    {PAD_MIPIRX2N, 2, 6, PIN_FUNC_GPIO},
    {PAD_MIPIRX2P, 2, 7, PIN_FUNC_GPIO},
    {PAD_MIPIRX1N, 2, 8, PIN_FUNC_GPIO},
    {PAD_MIPIRX1P, 2, 9, PIN_FUNC_GPIO},
    {PAD_MIPIRX0N, 2, 10, PIN_FUNC_GPIO},
    {PAD_MIPIRX0P, 2, 11, PIN_FUNC_GPIO},
    {PAD_MIPI_TXM2, 2, 16, PIN_FUNC_GPIO},
    {PAD_MIPI_TXP2, 2, 17, PIN_FUNC_GPIO},
    {PAD_MIPI_TXM1, 2, 14, PIN_FUNC_GPIO},
    {PAD_MIPI_TXP1, 2, 15, PIN_FUNC_GPIO},
    {PAD_MIPI_TXM0, 2, 12, PIN_FUNC_GPIO},
    {PAD_MIPI_TXP0, 2, 13, PIN_FUNC_GPIO},
    {PAD_AUD_AINL_MIC, 2, 23, PIN_FUNC_GPIO},
    {PAD_AUD_AOUTR, 2, 24, PIN_FUNC_GPIO},
    {GPIO_RTX, 1, 23, PIN_FUNC_GPIO},
    {SD1_D2, 4, 19, PIN_FUNC_GPIO},
    {SD1_D1, 4, 20, PIN_FUNC_GPIO},
    {SD1_CLK, 4, 23, PIN_FUNC_GPIO},
    {SD1_CMD, 4, 22, PIN_FUNC_GPIO},
    {SD1_D0, 4, 21, PIN_FUNC_GPIO},
    {SD1_D3, 4, 18, PIN_FUNC_GPIO},
    {PWR_SEQ2, 4, 4, PIN_FUNC_GPIO},
    {PWR_GPIO2, 4, 2, PWR_GPIO2__PWR_GPIO_2},
    {MUX_SPI1_MISO, 1, 8, PIN_FUNC_GPIO},
    {MUX_SPI1_MOSI, 1, 7, PIN_FUNC_GPIO},
    {MUX_SPI1_CS, 1, 10, PIN_FUNC_GPIO},
    {MUX_SPI1_SCK, 1, 9, PIN_FUNC_GPIO},
    {0xffffffffU, 0xfff, 0xfff}
};

dev_pin_map_t gpio_map[] = {
    {SD0_CLK, SD0_CLK__XGPIOA_7, 0},
    {SD0_CMD, SD0_CMD__XGPIOA_8, 0},
    {SD0_D0, SD0_D0__XGPIOA_9, 0},
    {SD0_D1, SD0_D1__XGPIOA_10, 0},
    {SD0_D2, SD0_D2__XGPIOA_11, 0},
    {SD0_D3, SD0_D3__XGPIOA_12, 0},
    {SD0_CD, SD0_CD__XGPIOA_13, 0},
    {SD0_PWR_EN, SD0_PWR_EN__XGPIOA_14, 0},
    {SPK_EN, SPK_EN__XGPIOA_15, 0},
    {UART0_TX, UART0_TX__XGPIOA_16, 0},
    {UART0_RX, UART0_RX__XGPIOA_17, 0},
    {JTAG_CPU_TCK, JTAG_CPU_TCK__XGPIOA_18, 0},
    {JTAG_CPU_TMS, JTAG_CPU_TMS__XGPIOA_19, 0},
    {EMMC_CLK, EMMC_CLK__XGPIOA_22, 0},
    {EMMC_CMD, EMMC_CMD__XGPIOA_23, 0},
    {EMMC_DAT1, EMMC_DAT1__XGPIOA_24, 0},
    {EMMC_DAT0, EMMC_DAT0__XGPIOA_25, 0},
    {EMMC_DAT2, EMMC_DAT2__XGPIOA_26, 0},
    {EMMC_DAT3, EMMC_DAT3__XGPIOA_27, 0},
    {IIC0_SCL, IIC0_SCL__XGPIOA_28, 0},
    {IIC0_SDA, IIC0_SDA__XGPIOA_29, 0},
    {AUX0, AUX0__XGPIOA_30, 0},
    {ADC1, ADC1__XGPIOB_3, 1},
    {USB_VBUS_DET, USB_VBUS_DET__XGPIOB_6, 1},
    {GPIO_RTX, GPIO_RTX__XGPIOB_23, 1},
    {PAD_ETH_TXM, PAD_ETH_TXM__XGPIOB_24, 1},
    {PAD_ETH_TXP, PAD_ETH_TXP__XGPIOB_25, 1},
    {PAD_ETH_RXM, PAD_ETH_RXM__XGPIOB_26, 1},
    {PAD_ETH_RXP, PAD_ETH_RXP__XGPIOB_27, 1},
    {PAD_MIPIRX4N, PAD_MIPIRX4N__XGPIOC_2, 2},
    {PAD_MIPIRX4P, PAD_MIPIRX4P__XGPIOC_3, 2},
    {PAD_MIPIRX3N, PAD_MIPIRX3N__XGPIOC_4, 2},
    {PAD_MIPIRX3P, PAD_MIPIRX3P__XGPIOC_5, 2},
    {PAD_MIPIRX2N, PAD_MIPIRX2N__XGPIOC_6, 2},
    {PAD_MIPIRX2P, PAD_MIPIRX2P__XGPIOC_7, 2},
    {PAD_MIPIRX1N, PAD_MIPIRX1N__XGPIOC_8, 2},
    {PAD_MIPIRX1P, PAD_MIPIRX1P__XGPIOC_9, 2},
    {PAD_MIPIRX0N, PAD_MIPIRX0N__XGPIOC_10, 2},
    {PAD_MIPIRX0P, PAD_MIPIRX0P__XGPIOC_11, 2},
    {PAD_MIPI_TXM1, PAD_MIPI_TXM1__XGPIOC_14, 2},
    {PAD_MIPI_TXP1, PAD_MIPI_TXP1__XGPIOC_15, 2},
    {PAD_AUD_AINL_MIC, PAD_AUD_AINL_MIC__XGPIOC_23, 2},
    {PAD_AUD_AOUTR, PAD_AUD_AOUTR__XGPIOC_24, 2},
    {SD1_D2, SD1_D2__PWR_GPIO_19, 4},
    {SD1_D1, SD1_D1__PWR_GPIO_20, 4},
    {SD1_CLK, SD1_CLK__PWR_GPIO_23, 4},
    {SD1_CMD, SD1_CMD__PWR_GPIO_22, 4},
    {SD1_D0, SD1_D0__PWR_GPIO_21, 4},
    {SD1_D3, SD1_D3__PWR_GPIO_18, 4},
    {PWR_SEQ2, PWR_SEQ2__PWR_GPIO_4, 4},
    {PWR_GPIO2, PWR_GPIO2__PWR_GPIO_2, 4},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t pwm_map[] = {
    {UART0_TX, UART0_TX__PWM_4, 4},
    {SD1_D3, SD1_D3__PWM_4, 4},
    {UART0_RX, UART0_RX__PWM_5, 5},
    {SD1_D2, SD1_D2__PWM_5, 5},
    {SD1_D1, SD1_D1__PWM_6, 6},
    {JTAG_CPU_TCK, JTAG_CPU_TCK__PWM_6, 6},
    {JTAG_CPU_TMS, JTAG_CPU_TMS__PWM_7, 7},
    {SD1_D0, SD1_D0__PWM_7, 7},
    {SD1_CMD, SD1_CMD__PWM_8, 8},
    {SD1_CLK, SD1_CLK__PWM_9, 9},
    {SD0_D3, SD0_D3__PWM_10, 10},
    {PAD_MIPI_TXM1, PAD_MIPI_TXM1__PWM_10, 10},
    {SD0_D2, PAD_MIPI_TXP1__PWM_11, 11},
    {SD0_D2, SD0_D2__PWM_11, 11},
    {PAD_ETH_TXM, PAD_ETH_TXM__PWM_12, 12},
    {SD0_D1, SD0_D1__PWM_12, 12},
    {PAD_ETH_TXP, PAD_ETH_TXP__PWM_13, 13},
    {SD0_D0, SD0_D0__PWM_13, 13},
    {PAD_ETH_RXM, PAD_ETH_RXM__PWM_14, 14},
    {SD0_CMD, SD0_CMD__PWM_14, 14},
    {PAD_ETH_TXP, PAD_ETH_RXP__PWM_15, 15},
    {SD0_CLK, SD0_CLK__PWM_15, 15},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t adc_map[] = {
    {ADC1, ADC1__XGPIOB_3, 0},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t uart_rx_map[] = {
    {UART0_RX, UART0_RX__UART0_RX, 0},
    {SD0_D2, SD0_D2__UART1_RX, 1},
    {UART0_RX, UART0_RX__UART1_RX, 1},
    {JTAG_CPU_TCK, JTAG_CPU_TCK__UART1_RX, 1},
    {IIC0_SDA, IIC0_SDA__UART1_RX, 1},
    {IIC0_SDA, IIC0_SDA__UART2_RX, 2},
    {SD1_D1, SD1_D1__UART2_RX, 2},
    {PAD_ETH_TXP, PAD_ETH_TXP__UART3_RX, 3},
    {SD1_D1, SD1_D1__UART3_RX, 3},
    {SD0_D3, SD0_D3__UART3_RX, 3},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t uart_tx_map[] = {
    {UART0_TX, UART0_TX__UART0_TX, 0},
    {SD0_D1, SD0_D1__UART1_TX, 1},
    {JTAG_CPU_TMS, JTAG_CPU_TMS__UART1_TX, 1},
    {UART0_TX, UART0_TX__UART1_TX, 1},
    {IIC0_SCL, IIC0_SCL__UART1_TX, 1},
    {IIC0_SCL, IIC0_SCL__UART2_TX, 2},
    {SD1_D2, SD1_D2__UART2_TX, 2},
    {PAD_ETH_RXP, PAD_ETH_RXP__UART3_TX, 3},
    {SD1_D2, SD1_D2__UART3_TX, 3},
    {SD0_D0, SD0_D0__UART3_TX, 3},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t iic_sda_map[] = {
    {PAD_MIPIRX4N, PAD_MIPIRX4N__IIC1_SDA, 1},
    {PAD_MIPIRX1P, PAD_MIPIRX1P__IIC1_SDA, 1},
    {SD1_D0, SD1_D0__IIC1_SDA, 1},
    {SD0_CLK, SD0_CLK__IIC1_SDA, 1},
    {SD0_D1, SD0_D1__IIC1_SDA, 1},
    {SD1_D1, SD1_D1__IIC1_SDA, 1},
    {PAD_MIPI_TXM1, PAD_MIPI_TXM1__IIC2_SDA, 2},
    {SD1_CLK, SD1_CLK__IIC3_SDA, 3},
    {PAD_MIPIRX2P, PAD_MIPIRX2P__IIC4_SDA, 4},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t iic_scl_map[] = {
    {PAD_MIPIRX4P, PAD_MIPIRX4P__IIC1_SCL, 1},
    {PAD_MIPIRX0N, PAD_MIPIRX0N__IIC1_SCL, 1},
    {SD1_D3, SD1_D3__IIC1_SCL, 1},
    {SD0_CMD, SD0_CMD__IIC1_SCL, 1},
    {SD0_D2, SD0_D2__IIC1_SCL, 1},
    {SD1_D2, SD1_D2__IIC1_SCL, 1},
    {PAD_MIPI_TXP1, PAD_MIPI_TXP1__IIC2_SCL, 2},
    {SD1_CMD, SD1_CMD__IIC3_SCL, 3},
    {PAD_MIPIRX2N, PAD_MIPIRX2N__IIC4_SCL, 4},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t spi_cs_map[] = {
    {SD0_D3, SD0_D3__SPI0_CS_X, 0},
    {PAD_ETH_RXM, PAD_ETH_RXM__SPI1_CS_X, 1},
    {SD1_D3, SD1_D3__SPI2_CS_X, 2},
    {0xFFFFFFFF, 0xFF, 0}};

dev_pin_map_t spi_sck_map[] = {
    {SD0_CLK, SD0_CLK__SPI0_SCK, 2, 0},
    {PAD_ETH_RXP, PAD_ETH_RXP__SPI1_SCK, 1},
    {SD1_CLK, SD1_CLK__SPI2_SCK, 2},
    {0xFF, 0xFFFFFFFF, 0}};

dev_pin_map_t spi_miso_map[] = {
    {SD0_D0, SD0_D0__SPI0_SDI, 2, 0},
    {PAD_ETH_TXM, PAD_ETH_TXM__SPI1_SDI, 1},
    {SD1_D0, SD1_D0__SPI2_SDI, 2},
    {0xFF, 0xFFFFFFFF, 0}};

dev_pin_map_t spi_mosi_map[] = {
    {SD0_CMD, SD0_CMD__SPI0_SDO, 2, 0},
    {PAD_ETH_TXP, PAD_ETH_TXP__SPI1_SDO, 1},
    {SD1_CMD, SD1_CMD__SPI2_SDO, 2},
    {0xFF, 0xFFFFFFFF, 0}};

pin_name_t out_pin_map[] = {
    PWR_GPIO2,
    IIC0_SCL,
    IIC0_SDA,
    0xFF,
    JTAG_CPU_TMS,
    JTAG_CPU_TCK,
    SD1_D2,
    SD1_D1,
    0xFF,
    SD1_CLK,
    SD1_CMD,
    SD1_D0,
    SD1_D3,
    0xFF,
    PAD_MIPI_TXM1,
    PAD_MIPI_TXP1,
    UART0_TX,
    UART0_RX,
    0xFF,
    SD0_PWR_EN,
    SPK_EN,
    EMMC_CMD,
    EMMC_DAT1,
    0xFF,
    EMMC_CLK,
    EMMC_DAT0,
    EMMC_DAT3,
    EMMC_DAT2,
    0xFF,
    PWR_SEQ2,
    0xFF,
    ADC1,
    USB_VBUS_DET,
    0xFF,
    PAD_AUD_AOUTR,
    GPIO_RTX,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF
};

pin_func_t out_pin_func_map[] = {
    PWR_GPIO2__PWR_GPIO_2,
    IIC0_SCL__XGPIOA_28,
    IIC0_SDA__XGPIOA_29,
    0xFF,
    JTAG_CPU_TMS__XGPIOA_19,
    JTAG_CPU_TCK__XGPIOA_18,
    SD1_D2__PWR_GPIO_19,
    SD1_D1__PWR_GPIO_20,
    0xFF,
    SD1_CLK__PWR_GPIO_23,
    SD1_CMD__PWR_GPIO_22,
    SD1_D0__PWR_GPIO_21,
    SD1_D3__PWR_GPIO_18,
    0xFF,
    PAD_MIPI_TXM1__XGPIOC_14,
    PAD_MIPI_TXP1__XGPIOC_15,
    UART0_TX__XGPIOA_16,
    UART0_RX__XGPIOA_17,
    0xFF,
    SD0_PWR_EN__XGPIOA_14,
    SPK_EN__XGPIOA_15,
    EMMC_CMD__XGPIOA_23,
    EMMC_DAT1__XGPIOA_24,
    0xFF,
    EMMC_CLK__XGPIOA_22,
    EMMC_DAT0__XGPIOA_25,
    EMMC_DAT3__XGPIOA_27,
    EMMC_DAT2__XGPIOA_26,
    0xFF,
    PWR_SEQ2__PWR_GPIO_4,
    0xFF,
    ADC1__XGPIOB_3,
    USB_VBUS_DET__XGPIOB_6,
    0xFF,
    PAD_AUD_AOUTR__XGPIOC_24,
    GPIO_RTX__XGPIOB_23,
    0xFF,
    0xFF,
    0xFF,
    0xFF,
    0xFF
};
