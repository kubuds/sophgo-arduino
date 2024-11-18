#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <syslog.h>
#include <pthread.h>

#include "global.h"
#include "queue.h"
#include "task.h"
#include "zlib.h"

void *g_handlertid;
static int signal_flag = 0;

#define BUFSIZE 64
static int calc_img_crc32(const char *in_file, uint32_t *img_crc)
{
    FILE *fp;
    size_t count;
    uint8_t buf[BUFSIZE] = {0};

    uint32_t crc = crc32(0L, Z_NULL, 0);

    fp = fopen(in_file, "rb");

    while (!feof(fp))
    {
        memset(buf, 0, sizeof(buf));
        count = fread(buf, sizeof(char), sizeof(buf), fp);
        crc = crc32(crc, buf, count);
    }
    *img_crc = crc;

    fclose(fp);

    return 0;
}

static int is_state_offline(int fd)
{
    char buf[8];
    int count;
    count = read(fd, buf, 7);
    buf[7] = '\0';
    if (count < 0)
        return -1;

    if (strcmp(buf, "offline") != 0)
        return 1;

    return 0;
}

static int is_state_running(int fd)
{
    char buf[8];
    int count;
    count = read(fd, buf, 7);
    buf[7] = '\0';
    if (count < 0)
        return -1;

    if (strcmp(buf, "running") != 0)
        return 1;

    return 0;
}

int echo_firmware()
{
    static int ret = TK_OK;
    int firmware_fd;
    firmware_fd = open(SYSFS_FIRMWARE, O_RDWR);

    if (firmware_fd < 0)
    {
        pr_log(LOG_ERR, "firmware_fd TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        goto err;
    }

    if (write(firmware_fd, FIRMWARE_NAME, 11) < 0)
    {
        pr_log(LOG_ERR, "write TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
    }

    close(firmware_fd);

err:
    return ret;
}

int ctrl_firmware(int flag)
{
    static int ret = TK_TIMEOUT;
    int count = 0;
    int state_fd;

    state_fd = open(SYSFS_STATE, O_RDWR);
    if (state_fd < 0)
    {
        pr_log(LOG_ERR, "firmware_fd TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        goto err;
    }

    if (flag == FIRMWARE_OFF && is_state_running(state_fd) == 0)
    {
        count = write(state_fd, "stop", 4);
    }

    if (flag == FIRMWARE_ON && is_state_offline(state_fd) == 0)
    {
        count = write(state_fd, "start", 5);
    }

    sleep(0.5);

    if (count < 0)
    {
        pr_log(LOG_ERR, "write TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
    }

    if (flag == FIRMWARE_OFF && is_state_offline(state_fd) == 0)
    {
        ret = TK_OK;
    }

    if (flag == FIRMWARE_ON && is_state_running(state_fd) == 0)
    {
        ret = TK_OK;
    }

err:
    return ret;
}

void *start_task(void *arg)
{
    static int ret = TK_OK;
    // int firmware_fd;
    int state_fd;
    char buf[8];

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    ret = echo_firmware();
    if (ret != TK_OK)
    {
        pr_log(LOG_INFO, "echo firmware faild...");
        goto err;
    }
    // firmware_fd = open(SYSFS_FIRMWARE, O_RDWR);
    // if (firmware_fd < 0)
    // {
    //     pr_log(LOG_ERR, "firmware_fd TK_OPTS_ERR...");
    //     ret = TK_OPTS_ERR;
    //     goto err;
    // }

    // if (write(firmware_fd, FIRMWARE_NAME, 11) < 0)
    // {
    //     pr_log(LOG_ERR, "write TK_OPTS_ERR...");
    //     ret = TK_OPTS_ERR;
    //     close(firmware_fd);
    //     goto err;
    // }

    // close(firmware_fd);

    state_fd = open(SYSFS_STATE, O_RDWR);
    if (state_fd < 0)
    {
        pr_log(LOG_ERR, "open TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        goto err;
    }

    if (read(state_fd, buf, 7) > 0)
    {
        buf[7] = '\0';
        if (strcmp(buf, "offline") != 0)
        {
            pr_log(LOG_ERR, "firmware is running...");
            close(state_fd);
            goto err;
        }
    }

    if (write(state_fd, "start", 5) < 0)
    {
        pr_log(LOG_ERR, "write TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        close(state_fd);
        goto err;
    }

    close(state_fd);

err:
    pr_log(LOG_INFO, "start task exit...");
    pthread_exit((void *)&ret);
}

void *stop_task(void *arg)
{
    static int ret = TK_OK;
    int state_fd;
    char buf[8];

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    state_fd = open(SYSFS_STATE, O_RDWR);
    if (state_fd < 0)
    {
        pr_log(LOG_ERR, "open TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        goto err;
    }

    if (read(state_fd, buf, 7) > 0)
    {
        buf[7] = '\0';
        if (strcmp(buf, "running") != 0)
        {
            pr_log(LOG_ERR, "firmware is offline...");
            close(state_fd);
            goto err;
        }
    }

    if (write(state_fd, "stop", 4) < 0)
    {
        pr_log(LOG_ERR, "write TK_OPTS_ERR...");
        ret = TK_OPTS_ERR;
        close(state_fd);
        goto err;
    }

    close(state_fd);

err:
    pr_log(LOG_INFO, "stop task exit...");
    pthread_exit((void *)&ret);
}

void *update_task(void *arg)
{
    static int ret = TK_OK;
    uint32_t crc32 = *(uint32_t *)arg;
    size_t w_count = 0;
    size_t w_total = 0;
    struct bufferq *read_buffer = NULL;
    FILE *write_ptr;
    struct downloader_client *client = main_client;
    int result;
    struct stat firmware_st;

    uint32_t img_crc32;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (stat(FIRMWARE_DIR, &firmware_st) == -1)
    {
        result = mkdir(FIRMWARE_DIR, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
        if (result != 0)
        {
            ret = TK_OPTS_ERR;
            pr_log(LOG_ERR, "fail create /lib/firmware...");
            goto err;
        }
    }

    if (access(FIRMWARE_PATH, F_OK) != -1)
    {
        if (remove(REC_FIRMWARE_PATH) == 0)
        {
            pr_log(LOG_INFO, "remove old arduino firmware!");
        }
        else
        {
            pr_log(LOG_ERR, "no remove old arduino firmware!");
        }

        if (rename(FIRMWARE_PATH, REC_FIRMWARE_PATH) == 0)
        {
            pr_log(LOG_INFO, "backup arduino firmware!");
        }
        else
        {
            pr_log(LOG_ERR, "backup arduino firmware err!");
        }
    }

    write_ptr = fopen(FIRMWARE_PATH, "wb");
    while (1)
    {
        sem_wait(&sem);
        if (pthread_mutex_lock(&bf_mutex) != 0)
        {
            pr_log(LOG_ERR, "pthread_mutex_lock lock error");
        }

        read_buffer = TAILQ_FIRST(&client->buffer_eq);

        if (pthread_mutex_unlock(&bf_mutex) != 0)
        {
            pr_log(LOG_ERR, "pthread_mutex_lock unlock error");
        }

        if (read_buffer == NULL)
        {
            fclose(write_ptr);
            calc_img_crc32(FIRMWARE_PATH, &img_crc32);
            if (w_total == client->read_count && crc32 == img_crc32)
            {
                ret = TK_OK;
            }
            else
            {
                ret = TK_DATA_ERR;
            }
            break;
        }

        {
            w_count = fwrite(read_buffer->buf, sizeof(uint8_t), read_buffer->len, write_ptr);
            if (w_count < 0)
            {
                fclose(write_ptr);
                break;
            }
            else
            {
                w_total += w_count;
            }
        }

        if (pthread_mutex_lock(&bf_mutex) != 0)
        {
            pr_log(LOG_ERR, "pthread_mutex_lock lock error");
        }

        TAILQ_REMOVE(&client->buffer_eq, read_buffer, entries);

        if (pthread_mutex_unlock(&bf_mutex) != 0)
        {
            pr_log(LOG_ERR, "pthread_mutex_lock unlock error");
        }

        free(read_buffer->buf);
        read_buffer->buf = NULL;
        free(read_buffer);
        read_buffer = NULL;
        sem_post(&sem);
    }

err:
    pr_log(LOG_INFO, "update task exit...");
    pthread_exit((void *)&ret);
}

void timeoutHandler()
{
    if (g_handlertid == NULL)
    {
        return;
    }
    pthread_t *p = g_handlertid;
    int ret = pthread_cancel(*p);
    if (ret != 0)
    {
        pr_log(LOG_INFO, "pthread_cancel error %x...", ret);
    }
}

void ctrlcHandler()
{
    if (g_handlertid != NULL)
    {
        pthread_t *p = g_handlertid;
        int ret = pthread_cancel(*p);
        if (ret != 0)
        {
            pr_log(LOG_INFO, "pthread_cancel error %x...", ret);
        }
    }
    pr_log(LOG_INFO, "brund bye... ");
    exit(0);
}

void signalHandler(int arg)
{
    switch (arg)
    {
    case SIGALRM:
        timeoutHandler();
        break;
    case SIGINT:
    case SIGKILL:
    case SIGHUP:
        ctrlcHandler();
        break;
    default:
        break;
    }
}

int run_task(enum TASK_CMD cmd, uint32_t param)
{
    int *pret = 0;
    int err = 0;
    pthread_t tid;
    uint32_t firmware_crc32;
    int dt = 0;

    openlog("brund", LOG_PID | LOG_NDELAY, LOG_USER);

    if (signal_flag == 0)
    {
        signal(SIGALRM, signalHandler);
        signal(SIGHUP, signalHandler);
        signal(SIGINT, signalHandler);
        signal(SIGKILL, signalHandler);
        signal_flag = 1;
    }

    g_handlertid = &tid;

    switch (cmd)
    {
    case START:
        dt = alarm(3);
        err = pthread_create(&tid, NULL, start_task, NULL);
        pr_log(LOG_INFO, "create start task...");
        break;
    case STOP:
        dt = alarm(4);
        err = pthread_create(&tid, NULL, stop_task, NULL);
        pr_log(LOG_INFO, "create stop task...");
        break;
    case UPDATE:
        firmware_crc32 = param;
        dt = alarm(20);
        err = pthread_create(&tid, NULL, update_task, (void *)&firmware_crc32);
        pr_log(LOG_INFO, "create update task...");
        break;
    default:
        break;
    }

    if (dt != 0)
    {
        pr_log(LOG_ERR, "alarm dt=%d error", dt);
        return TK_SCHED_ERR;
    }

    if (err != 0)
    {
        return TK_SCHED_ERR;
    }

    err = pthread_join(tid, (void **)&pret);
    if (pret == PTHREAD_CANCELED)
    {
        pr_log(LOG_INFO, "timeout task cancel...");
    }

    g_handlertid = NULL;
    dt = alarm(0);
    if (dt == 0)
    {
        pr_log(LOG_ERR, "alarm timeout");
        return TK_TIMEOUT;
    }
    else
    {
        pr_log(LOG_INFO, "task success alarm cancel...");
    }

    closelog();

    return *pret;
}