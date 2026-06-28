#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <signal.h>

#define SHM_NAME "/device_shm"

typedef struct {
    pthread_mutex_t mutex;
    int             status;
} device_state_t;

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

int main(void) {
    setbuf(stdout, NULL);

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        fprintf(stderr, "[Device] Error: Shared memory not initialized. Start controller first.\n");
        return EXIT_FAILURE;
    }

    device_state_t *state = (device_state_t *)mmap(NULL, sizeof(device_state_t),
                                                   PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    close(shm_fd);

    if (state == MAP_FAILED) {
        perror("[Device] mmap attach failure");
        return EXIT_FAILURE;
    }

    printf("[Device] Attached to %s\n", SHM_NAME);

    while (keep_running) {
        int local_status;

        /* Critical section isolation using the Process-Shared Mutex */
        pthread_mutex_lock(&state->mutex);
        local_status = state->status;
        pthread_mutex_unlock(&state->mutex);

        if (local_status == 1) {
            printf("[Device] Status: ON  — Running...\n");
        } else {
            printf("[Device] Status: OFF — Idle.\n");
        }

        unsigned int remaining = 1;
        while (remaining > 0 && keep_running) {
            remaining = sleep(remaining);
        }
    }

    munmap(state, sizeof(device_state_t));
    printf("\n[Device] Detached. Exiting safely.\n");
    return EXIT_SUCCESS;
}