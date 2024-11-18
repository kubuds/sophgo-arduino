#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <syslog.h>
#include <ev.h>

#include "global.h"
#include "burn_serial.h"
#include "data.h"
#include "task.h"

pthread_mutex_t bf_mutex;
sem_t sem;
struct downloader_client *main_client = NULL;

int main()
{
    openlog("brund", LOG_PID | LOG_NDELAY, LOG_USER);
    main_client = calloc(1, sizeof(*main_client));
    if (main_client == NULL)
    {
        pr_log(LOG_ERR, "malloc failed");
        goto err;
    }

    TAILQ_INIT(&main_client->buffer_eq);

    if (sem_init(&sem, 0, 0) < 0)
    {
        pr_log(LOG_ERR, "sem_init");
        goto err;
    }

    if (pthread_mutex_init(&bf_mutex, NULL) != 0)
    {
        pr_log(LOG_ERR, "pthread_mutex_init failed");
        goto err1;
    }

    if (0 != uart_init(&(main_client->fd), UART_SPEED, UART_DATA_BIT,
                       UART_STOP_BIT, UART_CHECK_BIT))
    {
        pr_log(LOG_ERR, "uart init failed");
        goto err1;
    }

    // atuo boot
    if (access(FIRMWARE_PATH, F_OK) == 0)
    {
        echo_firmware();
        if (ctrl_firmware(FIRMWARE_ON) == TK_OK)
            pr_log(LOG_INFO, "auto_boot arduino running...");
    }
    else
    {
        pr_log(LOG_INFO, "auto_boot arduino file not exists...");
    }

    main_client->loop = main_loop_init(main_client);
    if (NULL == main_client->loop)
    {
        pr_log(LOG_ERR, "loop init failed");
        goto err2;
    }
    ev_run(main_client->loop, 0);

err2:
    close(main_client->fd);
err1:
    sem_destroy(&sem);
err:
    closelog();
    return 0;
}
