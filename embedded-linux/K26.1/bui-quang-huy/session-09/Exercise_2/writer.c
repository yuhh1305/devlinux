#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "device_cfg.h"

#define CONFIG_FILE "/tmp/device.cfg"

const char* get_log_level_str(int level) {
    switch(level) {
        case 0: return "OFF";
        case 1: return "ERROR";
        case 2: return "INFO";
        case 3: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main(void) {
    setbuf(stdout, NULL);

    int fd = open(CONFIG_FILE, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("[Writer] open failed");
        return EXIT_FAILURE;
    }

    /* Ensure file size matches struct precisely */
    if (ftruncate(fd, sizeof(device_cfg_t)) < 0) {
        perror("[Writer] ftruncate failed");
        close(fd);
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd); /* File descriptor safe to close post-mmap mapping layer initialization */

    if (cfg == MAP_FAILED) {
        perror("[Writer] mmap failed");
        return EXIT_FAILURE;
    }

    /* Setup defaults if uninitialized */
    if (cfg->baud_rate == 0) {
        cfg->baud_rate = 9600;
        cfg->sampling_rate_hz = 100;
        cfg->log_level = 2; /* INFO */
        msync(cfg, sizeof(device_cfg_t), MS_SYNC);
    }

    printf("[Config Writer] Loaded %s\n", CONFIG_FILE);

    char input[64];
    while (1) {
        printf("\nCurrent: baud_rate=%d sampling_rate=%d log_level=%s\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, get_log_level_str(cfg->log_level));
        printf("Select field to update [baud/rate/log/quit]: ");
        
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "baud") == 0) {
            printf("Select baud rate [9600/115200/460800]: ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val == 9600 || val == 115200 || val == 460800) {
                    cfg->baud_rate = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] baud_rate = %d\n", val);
                } else {
                    printf("Invalid baud rate selection.\n");
                }
            }
        } else if (strcmp(input, "rate") == 0) {
            printf("Enter sampling rate hz (1-1000): ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val >= 1 && val <= 1000) {
                    cfg->sampling_rate_hz = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] sampling_rate_hz = %d\n", val);
                } else {
                    printf("Invalid range.\n");
                }
            }
        } else if (strcmp(input, "log") == 0) {
            printf("Select log level [0=OFF, 1=ERROR, 2=INFO, 3=DEBUG]: ");
            if (fgets(input, sizeof(input), stdin)) {
                int val = atoi(input);
                if (val >= 0 && val <= 3) {
                    cfg->log_level = val;
                    msync(cfg, sizeof(device_cfg_t), MS_SYNC);
                    printf("[Updated] log_level = %s\n", get_log_level_str(val));
                } else {
                    printf("Invalid log level.\n");
                }
            }
        } else {
            printf("Unknown selection command.\n");
        }
    }

    munmap(cfg, sizeof(device_cfg_t));
    return EXIT_SUCCESS;
}