#ifndef __CVISERIAL_H_
#define __CVISERIAL_H_

#define UART_PORT "/dev/ttyGS0"
#define UART_SPEED 115200
#define UART_DATA_BIT 8
#define UART_STOP_BIT 1
#define UART_CHECK_BIT 0

extern int uart_init(int *fd, int speed, int data_bit, int stop, int check);

#endif