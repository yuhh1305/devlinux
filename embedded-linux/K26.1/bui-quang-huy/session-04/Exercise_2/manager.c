/**
 * @file manager.c
 * @brief Parent program that acts as the supervisor shell.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#define INPUT_MAX_LEN 100
extern char **environ; /* Biến môi trường hệ thống bắt buộc cho execve */

int main(void) {
    char input_id[INPUT_MAX_LEN];
    int status;

    printf("\n=============================================\n");
    printf("   STUDENT LOOKUP SYSTEM — MANAGER\n");
    printf("   (fork + execve | file: students.txt)\n");
    printf("=============================================\n");
    printf("[MANAGER] PID: %d\n", getpid());

    while (1) {
        printf("Enter student ID ('quit' to exit).\n");
        printf("---------------------------------------------\n");
        printf("Student ID: ");
        
        if (fgets(input_id, sizeof(input_id), stdin) == NULL) {
            break;
        }

        /* Xử lý ký tự xuống dòng từ bước nhấn Enter */
        input_id[strcspn(input_id, "\n")] = '\0';

        /* Điều kiện thoát */
        if (strcmp(input_id, "quit") == 0) {
            printf("[MANAGER] Exiting. Goodbye!\n");
            break;
        }

        /* Bỏ qua nếu người dùng nhấn Enter mà không nhập gì */
        if (strlen(input_id) == 0) {
            continue;
        }

        printf("\n[MANAGER] fork() → child PID...\n");
        fflush(stdout);

        pid_t pid = fork();

        if (pid < 0) {
            perror("[MANAGER] Fork failed");
            continue;
        } 
        else if (pid == 0) {
            /* Nhánh tiến trình con: Thiết lập mảng đối số để gọi execve */
            char *args[] = {"./searcher", input_id, "students.txt", NULL};

            /*
             * Hệ thống nhúng nâng cao: execve() sẽ thay thế toàn bộ không gian 
             * địa chỉ (Code, Data, Stack, Heap) của tiến trình con hiện tại bằng binary mới.
             */
            execve(args[0], args, environ);

            /* * DÒNG DƯỚI ĐÂY CHỈ ĐƯỢC CHẠY KHI EXECVE THẤT BẠI.
             * Lý do: Nếu execve thành công, lõi thực thi của tiến trình con đã bị 
             * thay thế hoàn toàn bằng chương trình "searcher", nên mã lệnh phía dưới 
             * không còn tồn tại trong RAM của tiến trình này nữa.
             */
            perror("[MANAGER ERROR] execve execution failed");
            exit(2);
        } 
        else {
            /* Nhánh cha: Chờ tiến trình con thực hiện tra cứu xong */
            printf("[MANAGER] Waiting for child (waitpid)... \n");
            pid_t child_pid = waitpid(pid, &status, 0);

            if (child_pid > 0 && WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("[MANAGER] Child (PID %d) exited. code=%d → ", child_pid, exit_code);
                
                switch(exit_code) {
                    case 0:
                        printf("Found\n");
                        break;
                    case 1:
                        printf("Not found\n");
                        break;
                    case 2:
                        printf("File or argument error\n");
                        break;
                    default:
                        printf("Unknown error\n");
                }
            } else {
                printf("[MANAGER] Child process monitored abnormal termination.\n");
            }
            printf("---------------------------------------------\n");
        }
    }

    return EXIT_SUCCESS;
}