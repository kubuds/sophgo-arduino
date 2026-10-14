#ifndef VARIANT_H
#define VARIANT_H
/* SoC selection: picks cores/sg200x/soc/cv1800b/ and its headers. */
#define SOC_CV1800B            1
#include "soc/cv1800b/soc_pins.h"
#include "soc/sg200x_common.h"
#include "variant_pins.h"

#define DEBUG_SERIAL            Serial3
#define DEBUG_UART_BASE         DW_UART3_BASE

#define WIRE_INTERFACES_COUNT       4
#define OUT_PIN_NUM                 41

#define digitalPinToInterrupt(p)    (((p) < CONFIG_GPIO_NUM * 32) ? (p) : -1)

#define SERIAL_INTERFACES_COUNT 5

#define Wire Wire0
#endif
