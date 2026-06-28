#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    (void)sig;
    keep_running = 0;
}

/* Helper function to read CPU Load Average */
void get_cpu_load(char *res_buf, size_t max_len) {
    double load1 = 0.0;
    FILE *f = fopen("/proc/loadavg", "r");
    if (f) {
        if (fscanf(f, "%lf", &load1) != 1) {
            load1 = 0.0;
        }
        fclose(f);
    }
    snprintf(res_buf, max_len, "load_avg=%.2f", load1);
}

/* Helper function to read Memory Metrics safely */
void get_mem_info(char *res_buf, size_t max_len) {
    long mem_total = 0;
    long mem_free = 0;
    char line[256];
    
    FILE *f = fopen("/proc/meminfo", "r");
    if (f) {
        while (fgets(line, sizeof(line), f)) {
            if (sscanf(line, "MemTotal: %ld", &mem_total) == 1) continue;
            if (sscanf(line, "MemFree: %ld", &mem_free) == 1) continue;
        }
        fclose(f);
    }
    snprintf(res_buf, max_len, "mem_total=%ld kB mem_free=%ld kB", mem_total, mem_free);
}

int main(void) {
    setbuf(stdout, NULL);

    /* Setup SIGINT gracefully using sigaction */
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("[Daemon] sigaction failed");
        return EXIT_FAILURE;
    }

    /* Unlink stale socket path if any on startup */
    unlink(SOCKET_PATH);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("[Daemon] Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[Daemon] Bind failed");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if (listen(server_fd, 5) < 0) {
        perror("[Daemon] Listen failed");
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("[Daemon] Listening on %s...\n", SOCKET_PATH);

    while (keep_running) {
        struct sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue; /* Interrupted by SIGINT, loop condition will handle it */
            perror("[Daemon] Accept failed");
            break;
        }

        printf("[Daemon] Client connected.\n");

        char buffer[BUFFER_SIZE];
        while (keep_running) {
            memset(buffer, 0, sizeof(buffer));
            ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
            
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("[Daemon] Recv failed");
                break;
            } else if (n == 0) {
                printf("[Daemon] Client disconnected. Waiting for next client...\n");
                break;
            }

            /* Sanitize string inputs: strip trailing newlines or carriages */
            buffer[strcspn(buffer, "\r\n")] = '\0';

            if (strlen(buffer) == 0) continue;

            printf("[Daemon] CMD: %s\n", buffer);
            char response[BUFFER_SIZE] = {0};

            if (strcmp(buffer, "cpu") == 0) {
                get_cpu_load(response, sizeof(response));
            } else if (strcmp(buffer, "mem") == 0) {
                get_mem_info(response, sizeof(response));
            } else {
                snprintf(response, sizeof(response), "ERROR: unknown command");
            }

            if (send(client_fd, response, strlen(response), 0) < 0) {
                perror("[Daemon] Send failed");
                break;
            }
        }
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    printf("[Daemon] Socket unlinked clean. Exiting.\n");
    return EXIT_SUCCESS;
}