#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#include "queue.h"

#define CMD_PACKAGE_SIZE (8)
#define DATA_PACKAGE_SIZE (64)

enum TASK_ERRNO
{
    TK_OK = 0,
    TK_TIMEOUT,
    TK_START_ERR,
    TK_STOP_ERR,
    TK_OPTS_ERR,
    TK_SCHED_ERR,
    TK_CHECK_ERR,
    TK_DATA_ERR,
};

enum TASK_CMD
{
    NONE = 0,
    CONNECT,
    STOP,
    START,
    UPDATE,
    ERROR,
};

typedef struct __attribute__((packed)) __PACKAGE_HEAD_
{
    uint8_t frame_head[2];
    uint16_t frame_cmd;
    uint32_t length;
} package_head;

union packageHead
{
    package_head data;
    uint8_t buf[sizeof(package_head)];
};

typedef struct __attribute__((packed)) __PACKAGE_REPLY_
{
    uint8_t frame_head[2];
    uint16_t frame_cmd;
    uint16_t frame_reply;
    uint16_t cmd_errno;
} package_reply;

union packageReply
{
    package_reply data;
    uint8_t buf[sizeof(package_reply)];
};

typedef struct __attribute__((packed)) __PACKAGE_TAIL_
{
    uint32_t crc32;
    uint32_t frame_tail;
} package_tail;

union packageTail
{
    package_tail data;
    uint8_t buf[sizeof(package_tail)];
};

struct bufferq
{
    uint8_t *buf;
    int len;
    int offset;
    TAILQ_ENTRY(bufferq)
    entries;
};

struct downloader_client
{
    int fd;
    struct ev_loop *loop;
    uint32_t read_count;
    TAILQ_HEAD(, bufferq)
    buffer_eq;
};

#endif /* __TYPE_H__ */