/**
 * @file manager.c
 * @brief Multi-process order processing system simulation.
 * @author Bùi Quang Huy (Senior Embedded Software Engineer)
 * @date 2026-06-13
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define NUM_ORDERS 3

typedef struct {
    int   id;
    char  name[50];
    int   quantity;
    float unit_price;
} Order;

/* Hàm xử lý đơn hàng của tiến trình con */
void process_order(Order o) {
    float total = o.quantity * o.unit_price;
    printf("[CHILD-%d] PID: %d | PPID: %d\n", o.id, getpid(), getppid());
    printf("[CHILD-%d] %s x%d — Total: %.0f VND\n", o.id, o.name, o.quantity, total);
    printf("[CHILD-%d] Processing... (sleep 2s)\n\n", o.id);
    
    /* Mô phỏng thời gian xử lý phần cứng hoặc I/O */
    sleep(2);
}

int main(void) {
    Order orders[NUM_ORDERS] = {
        {1, "Backpack", 2, 350000},
        {2, "Shoes",    1, 500000},
        {3, "Hat",      3, 120000}
    };

    pid_t pids[NUM_ORDERS];
    int status;
    
    /* Biến thống kê cho tiến trình cha */
    int successful_orders = 0;
    int failed_orders = 0;
    float total_revenue = 0;

    printf("\n===================================================\n");
    printf("   ORDER PROCESSING SYSTEM — MANAGER (fork+wait)\n");
    printf("===================================================\n");
    printf("[MANAGER] PID: %d — spawning %d child processes...\n\n", getpid(), NUM_ORDERS);

    /* LOOP 1: Khởi tạo các tiến trình con xử lý song song (Concurrency) */
    for (int i = 0; i < NUM_ORDERS; i++) {
        /* * CRITICAL embedded note: Xóa bộ đệm stdout trước khi fork.
         * Nếu không, dữ liệu trong buffer của cha sẽ bị copy sang con, 
         * dẫn đến việc in trùng lặp dữ liệu log.
         */
        fflush(stdout); 

        pid_t pid = fork();

        if (pid < 0) {
            perror("[MANAGER] ERROR: Failed to fork child process");
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) {
            /* Nhánh của Tiến trình con (Child process) */
            process_order(orders[i]);
            /* Kết thúc tiến trình con an toàn với mã thoát 0 (Success) */
            exit(EXIT_SUCCESS); 
        } 
        else {
            /* Nhánh của Tiến trình cha (Parent process) */
            pids[i] = pid;
            printf("[MANAGER] fork() order #%d → child PID: %d\n", orders[i].id, pid);
        }
    }

    printf("[MANAGER] All %d children spawned. Starting waitpid()...\n\n", NUM_ORDERS);

    /* LOOP 2: Đồng bộ hóa - Chờ tất cả các tiến trình con kết thúc */
    for (int i = 0; i < NUM_ORDERS; i++) {
        /* Chờ đích danh từng PID theo thứ tự mảng */
        pid_t waited_pid = waitpid(pids[i], &status, 0);
        
        if (waited_pid > 0) {
            /* Kiểm tra xem tiến trình con có thoát bình thường không */
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("[MANAGER] waitpid(%d) — order #%d: exit code=%d → ", 
                       waited_pid, orders[i].id, exit_code);
                
                if (exit_code == 0) {
                    printf("SUCCESS\n");
                    successful_orders++;
                    total_revenue += (orders[i].quantity * orders[i].unit_price);
                } else {
                    printf("FAILED\n");
                    failed_orders++;
                }
            } else {
                printf("[MANAGER] waitpid(%d) — child terminated abnormally.\n", waited_pid);
                failed_orders++;
            }
        } else {
            perror("[MANAGER] ERROR: waitpid failed");
        }
    }

    /* In báo cáo tổng hợp */
    printf("\n================= SUMMARY =================\n");
    printf("  Total orders    : %d\n", NUM_ORDERS);
    printf("  Successful      : %d\n", successful_orders);
    printf("  Failed          : %d\n", failed_orders);
    printf("  Total revenue   : %.0f VND\n", total_revenue);
    printf("===========================================\n");

    return EXIT_SUCCESS;
}