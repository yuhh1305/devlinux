#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int             status;  /* 0 = OFF, 1 = ON */
} device_state_t;

int main(void) {
    setbuf(stdout, NULL);

    /* Unlink old shm if it exists from dangling crashes */
    shm_unlink(SHM_NAME);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("[Controller] shm_open failed");
        return EXIT_FAILURE;
    }

    if (ftruncate(shm_fd, sizeof(device_state_t)) < 0) {
        perror("[Controller] ftruncate failed");
        close(shm_fd);
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (state == MAP_FAILED) {
        perror("[Controller] mmap failed");
        return EXIT_FAILURE;
    }

    /* Configure Cross-Process Mutex Attribute */
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        perror("[Controller] mutexattr_init failed");
        munmap(state, sizeof(device_state_t));
        return EXIT_FAILURE;
    }
    
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED) != 0) {
        perror("[Controller] mutexattr_setpshared failed");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        return EXIT_FAILURE;
    }

    if (pthread_mutex_init(&state->mutex, &attr) != 0) {
        perror("[Controller] Mutex initialization failed");
        pthread_mutexattr_destroy(&attr);
        munmap(state, sizeof(device_state_t));
        return EXIT_FAILURE;
    }
    pthread_mutexattr_destroy(&attr); /* No longer needed after init */

    state->status = 0; /* Default OFF state */
    printf("[Controller] Shared memory ready. Commands: on / off / quit\n");

    char input[32];
    while (1) {
        printf("> ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\r\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "on") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 1;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: ON\n");
        } else if (strcmp(input, "off") == 0) {
            pthread_mutex_lock(&state->mutex);
            state->status = 0;
            pthread_mutex_unlock(&state->mutex);
            printf("[Controller] Command sent: OFF\n");
        } else {
            printf("Unknown option context. Select on, off, or quit.\n");
        }
    }

    printf("[Controller] Cleaning up. Goodbye.\n");
    pthread_mutex_destroy(&state->mutex);
    munmap(state, sizeof(device_state_t));
    shm_unlink(SHM_NAME);

    return EXIT_SUCCESS;
}