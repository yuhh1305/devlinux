#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define COLLECTOR_IP   "127.0.0.1"
#define COLLECTOR_PORT 9000
#define BUFFER_SIZE    128

int main(void) {
    setbuf(stdout, NULL);

    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("[Sensor] Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in target_addr;
    memset(&target_addr, 0, sizeof(target_addr));
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(COLLECTOR_PORT);
    if (inet_pton(AF_INET, COLLECTOR_IP, &target_addr.sin_addr) <= 0) {
        perror("[Sensor] Invalid address/Address not supported");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    printf("[Sensor] Target collector: %s:%d\n", COLLECTOR_IP, COLLECTOR_PORT);

    int i;
    for (i = 1; i <= 5; i++) {
        double load1 = 0.0;
        long mem_total = 1; /* Avoid division by zero */
        long mem_free = 0;
        char line[256];

        /* 1. Calculate Temperature from /proc/loadavg */
        FILE *f_load = fopen("/proc/loadavg", "r");
        if (f_load) {
            if (fscanf(f_load, "%lf", &load1) != 1) load1 = 0.0;
            fclose(f_load);
        }
        double temp = 40.0 + (load1 * 10.0);

        /* 2. Calculate Memory Usage Percent from /proc/meminfo */
        FILE *f_mem = fopen("/proc/meminfo", "r");
        if (f_mem) {
            while (fgets(line, sizeof(line), f_mem)) {
                if (sscanf(line, "MemTotal: %ld", &mem_total) == 1) continue;
                if (sscanf(line, "MemFree: %ld", &mem_free) == 1) continue;
            }
            fclose(f_mem);
        }
        if (mem_total == 0) mem_total = 1;
        double mem_used_pct = (double)(mem_total - mem_free) / mem_total * 100.0;

        /* 3. Construct Datagram payload safely */
        char payload[BUFFER_SIZE];
        snprintf(payload, sizeof(payload), "id=sensor-01 temp=%.1f mem_used=%.1f%%", temp, mem_used_pct);

        /* 4. Transmit via UDP datagram socket */
        if (sendto(sock_fd, payload, strlen(payload), 0, 
                   (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            perror("[Sensor] sendto failed");
            close(sock_fd);
            return EXIT_FAILURE;
        }

        printf("[Sent %d/5] %s\n", i, payload);
        
        if (i < 5) {
            sleep(2);
        }
    }

    close(sock_fd);
    printf("[Sensor] Done.\n");
    return EXIT_SUCCESS;
}