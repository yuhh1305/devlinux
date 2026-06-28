#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "sensor_shm.h"

int main(void) {
    setbuf(stdout, NULL);

    /* Try to get existing shm segment without IPC_CREAT flag */
    int shmid = shmget(SHM_KEY, sizeof(sensor_data_t), 0666);
    if (shmid < 0) {
        fprintf(stderr, "Daemon is not running.\n");
        return EXIT_FAILURE;
    }

    /* Attach to read */
    sensor_data_t *shm_data = (sensor_data_t *)shmat(shmid, NULL, SHM_RDONLY);
    if (shm_data == (void *)-1) {
        perror("shmat failed");
        return EXIT_FAILURE;
    }

    /* Print out structured data report */
    printf("[Sensor Report]\n");
    printf("Timestamp : %ld\n", (long)shm_data->timestamp);
    printf("CPU Temp  : %.2f C\n", shm_data->cpu_temp);
    printf("RAM Used  : %.2f %%\n", shm_data->ram_used_pct);

    /* Detach right away */
    if (shmdt(shm_data) < 0) {
        perror("shmdt failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}