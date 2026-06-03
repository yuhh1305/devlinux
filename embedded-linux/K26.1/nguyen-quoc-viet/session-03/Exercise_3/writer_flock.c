/*
 * writer_flock.c — log writer using flock() for mutual exclusion
 *
 * Comparison: flock vs fcntl
 * +---------------------------------+------------------+-----------------------------+
 * | Property                        | flock            | fcntl                       |
 * +---------------------------------+------------------+-----------------------------+
 * | Lock granularity                | Whole file only  | Byte range supported        |
 * | Works over NFS                  | No               | Yes                         |
 * | Inherited across fork           | Yes (shared FD)  | No (per open-file-desc)     |
 * | Automatically released on crash | Yes              | Yes                         |
 * | Best used when                  | Simple local FS  | Network FS / byte-range     |
 * +---------------------------------+------------------+-----------------------------+
 */

#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define LOG_FILE "system.log"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s \"message text\"\n", argv[0]);
        return 1;
    }

    int fd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    /* acquire exclusive lock — blocks until available */
    if (flock(fd, LOCK_EX) < 0) { perror("flock"); close(fd); return 1; }

    /* format log line inside the critical section */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", tm_info);

    char line[256];
    int len = snprintf(line, sizeof(line),
                       "[PID:%d] [%s] [INFO] %s\n",
                       (int)getpid(), timebuf, argv[1]);

    write(fd, line, (size_t)len);

    /* release lock */
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}
