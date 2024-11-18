#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <ev.h>

#include "queue.h"
#include "global.h"
#include "task.h"
#include "data.h"

static time_t updating_start;
static int updating = 0;
static size_t target_size = 0;
void watcher_uart_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
    void *user_data = ev_userdata(loop);
    uint8_t *buf = NULL;
    uint8_t recv_buf[DATA_PACKAGE_SIZE];
    struct bufferq *write_bufferq = NULL;
    struct bufferq *clean_bufferq = NULL;
    size_t read_size = 0;
    int ret = 0;
    enum TASK_CMD taskcmd;
    uint32_t crc32;
    double update_timeout;

    time_t nowtime;

    struct downloader_client *client = (struct downloader_client *)user_data;

    union packageHead head;
    union packageReply reply;
    union packageTail tail;

    openlog("brund", LOG_PID | LOG_NDELAY, LOG_USER);

    memset(recv_buf, '\0', DATA_PACKAGE_SIZE);
    read_size = read(client->fd, recv_buf, DATA_PACKAGE_SIZE);
    if (read_size <= DATA_PACKAGE_SIZE && read_size > 0)
    {

        if (recv_buf[0] == 0xAA && recv_buf[1] == 0x55 && updating == 1)
        {
            nowtime = time(NULL);
            update_timeout = difftime(nowtime, updating_start);

            if (update_timeout > 20.0f)
            {
                pr_log(LOG_INFO, "clean unused data");
                while (1)
                {
                    clean_bufferq = TAILQ_FIRST(&client->buffer_eq);
                    if (clean_bufferq == NULL)
                    {
                        pr_log(LOG_INFO, "bufferq clean success");
                        break;
                    }
                    TAILQ_REMOVE(&client->buffer_eq, clean_bufferq, entries);
                    free(clean_bufferq->buf);
                    clean_bufferq->buf = NULL;
                    free(clean_bufferq);
                    clean_bufferq = NULL;
                }
                client->read_count = 0;
                updating_start = 0;
                target_size = 0;
                updating = 0;
            }
        }

        if (recv_buf[0] == 0xAA && recv_buf[1] == 0x55 && updating == 0)
        {
            memcpy(head.buf, recv_buf, sizeof(package_head));
            taskcmd = (uint16_t)head.data.frame_cmd;

            reply.data.frame_head[0] = 0xAA;
            reply.data.frame_head[1] = 0x55;
            ret = 0;
            switch (taskcmd)
            {
            case NONE:
                pr_log(LOG_INFO, "Received CMD NONE");
                break;
            case CONNECT:
                pr_log(LOG_INFO, "Received CMD CONNECT");
                break;
            case STOP:
                pr_log(LOG_INFO, "Received CMD STOP");
                ret = run_task(STOP, 0);
                break;
            case START:
                pr_log(LOG_INFO, "Received CMD START");
                ret = run_task(START, 0);
                break;
            case UPDATE:
                pr_log(LOG_INFO, "Received CMD UPDATE");
                updating = 1;
                target_size = head.data.length;
                client->read_count = 0;
                updating_start = time(NULL);
                break;
            default:
                pr_log(LOG_INFO, "Received CMD NULL");
                taskcmd = ERROR;
                break;
            }
            reply.data.frame_cmd = taskcmd;
            reply.data.frame_reply = ret;
            reply.data.cmd_errno = ret;

            if (write(client->fd, reply.buf, sizeof(reply.buf)) != sizeof(reply.buf))
            {
                pr_log(LOG_DEBUG, "Write error");
            }
        }
        else
        {
            if (updating == 1 && client->read_count < target_size)
            {
                buf = malloc(read_size);
                if (buf == NULL)
                {
                    pr_log(LOG_DEBUG, "recv buffer malloc fail");
                }

                memcpy(buf, recv_buf, read_size);

                write_bufferq = calloc(1, sizeof(*write_bufferq));
                if (write_bufferq == NULL)
                {
                    pr_log(LOG_DEBUG, "bufferq malloc faild");
                }

                write_bufferq->buf = buf;
                write_bufferq->len = read_size;
                write_bufferq->offset = 0;

                if (pthread_mutex_lock(&bf_mutex) != 0)
                {
                    pr_log(LOG_DEBUG, "lock error");
                }

                TAILQ_INSERT_TAIL(&client->buffer_eq, write_bufferq, entries);
                client->read_count += read_size;

                if (pthread_mutex_unlock(&bf_mutex) != 0)
                {
                    pr_log(LOG_DEBUG, "unlock error");
                }
            }
            else
            {
                if (client->read_count == target_size)
                {

                    memcpy(tail.buf, recv_buf, sizeof(package_tail));

                    if (tail.buf[4] == 0xFF && tail.buf[5] == 0xFF && tail.buf[6] == 0xFF && tail.buf[7] == 0xFF)
                    {
                        crc32 = tail.data.crc32;
                        sem_post(&sem);
                        ret = run_task(UPDATE, crc32);

                        reply.data.frame_head[0] = 0xAA;
                        reply.data.frame_head[1] = 0x55;
                        reply.data.frame_cmd = UPDATE;
                        reply.data.frame_reply = ret;
                        reply.data.cmd_errno = 0;

                        if (write(client->fd, reply.buf, sizeof(reply.buf)) != sizeof(reply.buf))
                        {
                            pr_log(LOG_DEBUG, "write error");
                        }

                        client->read_count = 0;
                        updating_start = 0;
                        target_size = 0;
                        updating = 0;
                    }
                }
            }
        }
    }
    else
    {
        pr_log(LOG_INFO, "read CMD/Data error");
    }
    closelog();
}

struct ev_loop *main_loop_init(struct downloader_client *client)
{
    static struct ev_io uart_watcher;
    struct ev_loop *loop = ev_default_loop(EVBACKEND_POLL | EVBACKEND_SELECT | EVFLAG_NOENV);
    if (NULL == loop)
    {
        pr_log(LOG_DEBUG, "create loop failed");
        return NULL;
    }

    ev_set_userdata(loop, client);

    ev_io_init(&uart_watcher, watcher_uart_cb, client->fd, EV_READ);
    ev_io_start(loop, &uart_watcher);
    return loop;
}