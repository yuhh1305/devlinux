#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>

#include "device_cfg.h"

#define CONFIG_FILE "/tmp/device.cfg"

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

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

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    int fd = open(CONFIG_FILE, O_RDONLY);
    if (fd < 0) {
        perror("[Config Reader] Please start writer.c first to generate configuration file");
        return EXIT_FAILURE;
    }

    device_cfg_t *cfg = (device_cfg_t *)mmap(NULL, sizeof(device_cfg_t),
                                             PROT_READ, MAP_SHARED, fd, 0);
    close(fd);

    if (cfg == MAP_FAILED) {
        perror("[Config Reader] mmap layout configuration failed");
        return EXIT_FAILURE;
    }

    printf("[Config Reader] Polling %s every 2s...\n", CONFIG_FILE);

    while (keep_running) {
        printf("baud_rate=%-7d sampling_rate=%-3d Hz  log_level=%s\n", 
               cfg->baud_rate, cfg->sampling_rate_hz, get_log_level_str(cfg->log_level));
        
        unsigned int remaining = 2;
        while (remaining > 0 && keep_running) {
            remaining = sleep(remaining);
        }
    }

    munmap(cfg, sizeof(device_cfg_t));
    printf("\n[Config Reader] Gracefully detached mapping. Exiting.\n");
    return EXIT_SUCCESS;
}