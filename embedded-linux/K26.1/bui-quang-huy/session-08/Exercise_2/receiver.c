#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    256

int main(void) {
    setbuf(stdout, NULL);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("[Collector] Socket creation failed");
        return EXIT_FAILURE;
    }

    /* Enable SO_REUSEADDR to bypass "Address already in use" kernel lockouts */
    int opt = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("[Collector] setsockopt SO_REUSEADDR failed");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; /* Listen on all available interfaces (0.0.0.0) */
    serv_addr.sin_port = htons(COLLECTOR_PORT);

    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[Collector] Bind failed");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("[Collector] Listening on 0.0.0.0:%d...\n", COLLECTOR_PORT);

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recvfrom(sock_fd, buffer, sizeof(buffer) - 1, 0, 
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("[Collector] recvfrom failed");
            continue;
        }

        buffer[n] = '\0'; /* Ensure standard string null-termination */

        /* Capture timestamp formatting pattern: [HH:MM:SS] */
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char ts[16];
        if (t) {
            strftime(ts, sizeof(ts), "%H:%M:%S", t);
        } else {
            strncpy(ts, "00:00:00", sizeof(ts));
        }

        /* Extract peer network address data elements */
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, sizeof(ip_str));
        int port = ntohs(client_addr.sin_port);

        printf("[%s] %s:%d → %s\n", ts, ip_str, port, buffer);
    }

    close(sock_fd); /* Unreachable mathematically due to loop context, kept for compliance */
    return EXIT_SUCCESS;
}