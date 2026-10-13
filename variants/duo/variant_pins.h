#ifndef _VARIANT_PINS_H_
#define _VARIANT_PINS_H_

#include <stdint.h>

// User LED 
#define LED_BUILTIN 0
// ADC
#define A0 31

// GPIO
#define PIN_GPIOA23             (21)
#define PIN_GPIOA24             (22)
#define PIN_GPIOA22             (24)
#define PIN_GPIOA25             (25)
#define PIN_GPIOA26             (27)
#define PIN_GPIOA27             (26)
#define PIN_XGPIOA28            (1)
#define PIN_XGPIOA29            (2)
#define PIN_XGPIOB3             (31)
#define PIN_XGPIOB6             (32)

#define PIN_XGPIOC9             (14)
#define PIN_XGPIOC10            (15)

// PWM
#define PIN_PWM3                (31)
#define PIN_PWM4                (16)
#define PIN_PWM5                (17)
#define PIN_PWM6                (7)
#define PIN_PWM7                (11)
#define PIN_PWM8                (10)
#define PIN_PWM9                (9)
#define PIN_PWM10               (4)
#define PIN_PWM11               (5)

// ADC
#define PIN_ADC1                (31)

// UART
#define PIN_UART0_TX            (16)
#define PIN_UART0_RX            (17)
#define PIN_UART1_TX            (1)
#define PIN_UART1_RX            (2)
#define PIN_UART2_TX            (6)
#define PIN_UART2_RX            (7)
#define PIN_UART3_TX            (6)
#define PIN_UART3_RX            (7)
#define PIN_UART4_TX            (4)
#define PIN_UART4_RX            (5)

// IIC
#define PIN_IIC0_SDA            (2)
#define PIN_IIC0_SCL            (1)
#define PIN_IIC1_SDA            (11)
#define PIN_IIC1_SCL            (12)
#define PIN_IIC2_SDA            (0xff)
#define PIN_IIC2_SCL            (0xff)
#define PIN_IIC3_SDA            (9)
#define PIN_IIC3_SCL            (10)
#define PIN_IIC4_SDA            (0xff)
#define PIN_IIC4_SCL            (0xff)

// SPI
#define PIN_SPI2_CS             (12)
#define PIN_SPI2_SCK            (9)
#define PIN_SPI2_MISO           (11)
#define PIN_SPI2_MOSI           (10)

static const uint8_t KEY_BUILTIN = 21;

static const uint8_t TX = 16;
static const uint8_t RX = 17;

static const uint8_t SDA = 2;
static const uint8_t SCL = 1;

static const uint8_t SS = 12;
static const uint8_t MOSI = 10;
static const uint8_t MISO = 11;
static const uint8_t SCK = 9;

#endif /* _VARIANT_PINS_H_ */
