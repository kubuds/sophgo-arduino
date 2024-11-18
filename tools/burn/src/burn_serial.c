#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <syslog.h>
#include "burn_serial.h"
#include "global.h"

int uart_init(int *fd, int speed, int data_bit, int stop, int check)
{
    struct termios opt;
    *fd = open(UART_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == *fd)
    {
        pr_log(LOG_ERR, "open uart %s failed\r\n", UART_PORT);
        goto err1;
    }
    if (0 != tcgetattr(*fd, &opt))
    {
        goto err2;
    }
    opt.c_cflag &= ~CSIZE;
    switch (data_bit)
    {
    case 5:
        opt.c_cflag |= CS5;
        break;
    case 6:
        opt.c_cflag |= CS6;
        break;
    case 7:
        opt.c_cflag |= CS7;
        break;
    case 8:
        opt.c_cflag |= CS8;
        break;
    default:
        opt.c_cflag |= CS8;
        break;
    }
    if (2 == stop)
    {
        opt.c_cflag |= CSTOPB;
    }
    else
    {
        opt.c_cflag &= ~CSTOPB;
    }

    switch (check)
    {
    case 1:
        opt.c_cflag |= PARENB;
        opt.c_cflag |= PARODD;
        opt.c_iflag |= (INPCK | ISTRIP);
    case 2:
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
        opt.c_iflag |= (INPCK | ISTRIP);
    default:
        opt.c_cflag &= ~PARENB;
        break;
    }
    switch (speed)
    {
    case 19200:
        if (-1 == cfsetispeed(&opt, B19200))
        {
            goto err2;
        }
        if (-1 == cfsetospeed(&opt, B19200))
        {
            goto err2;
        }
        break;
    case 115200:
        if (-1 == cfsetispeed(&opt, B115200))
        {
            goto err2;
        }
        if (-1 == cfsetospeed(&opt, B115200))
        {
            goto err2;
        }
        break;
    default:
        if (-1 == cfsetispeed(&opt, B9600))
        {
            goto err2;
        }
        if (-1 == cfsetospeed(&opt, B9600))
        {
            goto err2;
        }
        break;
    }
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opt.c_oflag &= ~OPOST;
    opt.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IGNCR | IXON);
    tcflush(*fd, TCIOFLUSH);
    if (0 != tcsetattr(*fd, TCSANOW, &opt))
    {
        goto err2;
    }
    return 0;
err2:
    close(*fd);
err1:
    return -1;
}