#ifndef LOGGING_H
#define LOGGING_H

#include <time.h>
#include <linux/limits.h>
#include <pthread.h>

#define LOGLINE_LENGTH 256
#define LOGCACHE_COUNT 10

//from syslog.h
#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */

typedef struct
{
    const char *path;
    int split;
    int keep;
    int level;
} LogOption;

typedef struct
{
    char base[32];
    char path[PATH_MAX];
    time_t updated;
    LogOption opts;
    int cnt;
    char buf[LOGCACHE_COUNT][LOGLINE_LENGTH];
    pthread_mutex_t lock;
} Logger;

int log_init(Logger *log, const char *name, LogOption lo);
#endif
