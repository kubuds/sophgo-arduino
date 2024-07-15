#ifndef VARIANT_H
#define VARIANT_H
#include "variant_pins.h"
#include "../duo/variant_soc.h"

#define DEBUG_SERIAL            Serial3
#define DEBUG_UART_BASE         DW_UART3_BASE

#define CONFIG_BOARD_CV181XH        1
#define WIRE_INTERFACES_COUNT       5
#define OUT_PIN_NUM                 53

#define digitalPinToInterrupt(p)    (((p) < CONFIG_GPIO_NUM * 32) ? (p) : -1)

#define SERIAL_INTERFACES_COUNT 4
#define Wire Wire1
#define PIN_SPI2_SCK PIN_SPI3_SCK
#define PIN_SPI2_MISO PIN_SPI3_MISO
#define PIN_SPI2_MOSI PIN_SPI3_MOSI
#define PIN_SPI2_CS PIN_SPI3_CS

#endif
