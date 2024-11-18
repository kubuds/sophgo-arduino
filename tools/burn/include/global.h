#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <semaphore.h>
#include "type.h"

extern struct downloader_client *main_client;
extern pthread_mutex_t bf_mutex;
extern sem_t sem;

#ifdef DEBUG
#define pr_log syslog
#else
#define pr_log(x, args...)
#endif

#endif /* __GLOBAL_H__ */
