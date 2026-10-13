#ifndef _VARIANT_PINS_H_
#define _VARIANT_PINS_H_

#include <stdint.h>

// User LED 
#define LED_BUILTIN 0

// ADC
#define A0 46

// GPIO
#define PIN_XGPIOA16             (8)
#define PIN_XGPIOA17             (10)
#define PIN_XGPIOA18             (22)
#define PIN_XGPIOA19             (18)
#define PIN_XGPIOA20             (16)
#define PIN_XGPIOA28             (16)

#define PIN_XGPIOB11             (11)
#define PIN_XGPIOB12             (13)
#define PIN_XGPIOB13             (19)
#define PIN_XGPIOB14             (21)
#define PIN_XGPIOB15             (23)
#define PIN_XGPIOB16             (31)
#define PIN_XGPIOB18             (7)
#define PIN_XGPIOB19             (12)
#define PIN_XGPIOB20             (3)
#define PIN_XGPIOB21             (5)
#define PIN_XGPIOB22             (15)

#define PIN_XGPIOC9             (14)
#define PIN_XGPIOC10            (15)

// PWM
#define PIN_PWM1                (11)
#define PIN_PWM2                (13)
#define PIN_PWM3                (3)
#define PIN_PWM4                (8)
#define PIN_PWM5                (10)
#define PIN_PWM6                (22)
#define PIN_PWM7                (18)
#define PIN_PWM8                (40)
#define PIN_PWM9                (42)
#define PIN_PWM10               (44)
#define PIN_PWM12               (50)
#define PIN_PWM13               (48)

// ADC
#define PIN_ADC1                 (46)
#define PIN_ADC2                 (48)
#define PIN_ADC3                 (50)

// UART
#define PIN_UART0_TX            (8)
#define PIN_UART0_RX            (10)
#define PIN_UART1_TX            (18)
#define PIN_UART1_RX            (22)
#define PIN_UART2_TX            (23)
#define PIN_UART2_RX            (24)
#define PIN_UART3_TX            (50)
#define PIN_UART3_RX            (48)

// IIC
#define PIN_IIC0_SDA            (0xff)
#define PIN_IIC0_SCL            (0xff)
#define PIN_IIC1_SDA            (11)
#define PIN_IIC1_SCL            (13)
#define PIN_IIC2_SDA            (21)
#define PIN_IIC2_SCL            (19)
#define PIN_IIC3_SDA            (0xff)
#define PIN_IIC3_SCL            (0xff)
#define PIN_IIC4_SDA            (5)
#define PIN_IIC4_SCL            (3)

// SPI
#define PIN_SPI3_CS             (24)
#define PIN_SPI3_SCK            (23)
#define PIN_SPI3_MISO           (21)
#define PIN_SPI3_MOSI           (19)

// Default SPI pins (mapped to SPI3)
#define PIN_SPI2_CS             PIN_SPI3_CS
#define PIN_SPI2_SCK            PIN_SPI3_SCK
#define PIN_SPI2_MISO           PIN_SPI3_MISO
#define PIN_SPI2_MOSI           PIN_SPI3_MOSI

// Default SPI bus index
#define DEFAULT_SPI_BUS         3

//static const uint8_t KEY_BUILTIN = 21;

static const uint8_t TX = 16;
static const uint8_t RX = 17;

static const uint8_t SDA = 2;
static const uint8_t SCL = 1;

static const uint8_t SS = 23;
static const uint8_t MOSI = 19;
static const uint8_t MISO = 21;
static const uint8_t SCK = 24;

#endif /* _VARIANT_PINS_H_ */
