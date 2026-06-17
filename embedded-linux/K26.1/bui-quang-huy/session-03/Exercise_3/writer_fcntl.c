/*
| Property | fcntl |
|---|---|---|
| Lock granularity | Byte range supported |
| Works over NFS | Yes |
| Inherited across fork | No (per open-file-description) |
| Automatically released on crash | Yes |
| Best used when | Network FS or byte-range locking |
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Cách dùng: %s <tin_nhan_log>\n", argv[0]);
        return 1;
    }

    int fd = open("system.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Không mở được log file");
        return 1;
    }

    struct flock fl;
    memset(&fl, 0, sizeof(fl));
    fl.l_type   = F_WRLCK;    
    fl.l_whence = SEEK_SET;   
    fl.l_start  = 0;          
    fl.l_len    = 0;          

    if (fcntl(fd, F_SETLKW, &fl) < 0) {
        perror("Lỗi chiếm khóa fcntl");
        close(fd);
        return 1;
    }

    time_t rawtime;
    struct tm *timeinfo;
    char time_str[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    char log_buffer[512];
    snprintf(log_buffer, sizeof(log_buffer), "[PID:%d] [%s] [INFO] %s\n", getpid(), time_str, argv[1]);

    write(fd, log_buffer, strlen(log_buffer));

    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &fl) < 0) {
        perror("Lỗi giải phóng khóa fcntl");
    }

    close(fd);
    return 0;
}
