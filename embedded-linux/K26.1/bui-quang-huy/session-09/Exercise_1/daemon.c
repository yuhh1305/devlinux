#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

#include "sensor_shm.h"

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

void get_system_metrics(double *temp, double *mem_pct) {
    double load1 = 0.0;
    long mem_total = 1, mem_free = 0;
    char line[256];

    /* Read /proc/loadavg */
    FILE *f_load = fopen("/proc/loadavg", "r");
    if (f_load) {
        if (fscanf(f_load, "%lf", &load1) != 1) load1 = 0.0;
        fclose(f_load);
    }
    *temp = 40.0 + (load1 * 10.0);

    /* Read /proc/meminfo */
    FILE *f_mem = fopen("/proc/meminfo", "r");
    if (f_mem) {
        while (fgets(line, sizeof(line), f_mem)) {
            if (sscanf(line, "MemTotal: %ld", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %ld", &mem_free) == 1) continue;
        }
        fclose(f_mem);
    }
    if (mem_total <= 0) mem_total = 1;
    *mem_pct = (double)(mem_total - mem_free) / mem_total * 100.0;
}

int main(void) {
    setbuf(stdout, NULL);

    /* Register SIGINT via sigaction for safety */
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("[Daemon] sigaction failed");
        return EXIT_FAILURE;
    }

    /* Create Shared Memory Segment */
    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("[Daemon] shmget failed");
        return EXIT_FAILURE;
    }
    printf("[Daemon] Shared memory created. Key=0x%x\n", SHM_KEY);

    /* Attach to the Shared Memory Segment */
    sensor_data_t *shm_data = (sensor_data_t *)shmat(shmid, NULL, 0);
    if (shm_data == (void *)-1) {
        perror("[Daemon] shmat failed");
        return EXIT_FAILURE;
    }

    while (keep_running) {
        double temp = 0.0, mem_pct = 0.0;
        get_system_metrics(&temp, &mem_pct);

        /* Write data to memory mapped region */
        shm_data->timestamp = time(NULL);
        shm_data->cpu_temp = temp;
        shm_data->ram_used_pct = mem_pct;

        printf("[Daemon] Written: temp=%.2f ram=%.2f%%\n", temp, mem_pct);
        
        /* Sleep 2 seconds but handle interruption safely */
        unsigned int remaining = 2;
        while (remaining > 0 && keep_running) {
            remaining = sleep(remaining);
        }
    }

    /* Cleanup Section */
    printf("\n[Daemon] Cleaning up shared memory. Goodbye.\n");
    if (shmdt(shm_data) < 0) {
        perror("[Daemon] shmdt failed");
    }
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        perror("[Daemon] shmctl IPC_RMID failed");
    }

    return EXIT_SUCCESS;
}