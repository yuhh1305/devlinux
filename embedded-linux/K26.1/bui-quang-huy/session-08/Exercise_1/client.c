#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/monitor.sock"
#define BUFFER_SIZE 256

int main(void) {
    setbuf(stdout, NULL);

    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) {
        perror("[monitor-cli] Socket failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[monitor-cli] Connection failed");
        close(client_fd);
        return EXIT_FAILURE;
    }

    printf("[monitor-cli] Connected to %s\n", SOCKET_PATH);

    char cmd_buf[BUFFER_SIZE];
    char resp_buf[BUFFER_SIZE];

    while (1) {
        printf("> ");
        if (!fgets(cmd_buf, sizeof(cmd_buf), stdin)) {
            break;
        }

        /* Sanitize trailing newlines */
        cmd_buf[strcspn(cmd_buf, "\r\n")] = '\0';

        if (strcmp(cmd_buf, "quit") == 0) {
            break;
        }
        if (strlen(cmd_buf) == 0) {
            continue;
        }

        if (send(client_fd, cmd_buf, strlen(cmd_buf), 0) < 0) {
            perror("[monitor-cli] Send failed");
            break;
        }

        memset(resp_buf, 0, sizeof(resp_buf));
        ssize_t n = recv(client_fd, resp_buf, sizeof(resp_buf) - 1, 0);
        if (n < 0) {
            perror("[monitor-cli] Recv failed");
            break;
        } else if (n == 0) {
            printf("[monitor-cli] Server closed connection.\n");
            break;
        }

        printf("%s\n", resp_buf);
    }

    close(client_fd);
    return EXIT_SUCCESS;
}