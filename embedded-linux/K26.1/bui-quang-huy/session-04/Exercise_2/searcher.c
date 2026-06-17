/**
 * @file searcher.c
 * @brief Child program executed via execve to search database.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_MAX_LEN 256

/* Hàm phân loại học lực dựa trên GPA */
const char* get_grade_classification(float gpa) {
    if (gpa >= 8.5f) return "Excellent";
    if (gpa >= 7.0f) return "Good";
    if (gpa >= 5.0f) return "Average";
    return "Poor";
}

int main(int argc, char *argv[]) {
    /* * Kiểm tra đối số: 
     * argv[0] = target name, argv[1] = student_id, argv[2] = database path 
     */
    if (argc < 3) {
        fprintf(stderr, "[SEARCHER ERROR] Missing arguments.\n");
        exit(2); /* Exit code 2: Lỗi đối số/file */
    }

    char *search_id = argv[1];
    char *db_path = argv[2];

    printf("[SEARCHER] PID: %d | PPID: %d\n", getpid(), getppid());
    printf("[SEARCHER] Searching for \"%s\" in %s...\n", search_id, db_path);

    FILE *fp = fopen(db_path, "r");
    if (fp == NULL) {
        perror("[SEARCHER ERROR] Cannot open database file");
        exit(2); 
    }

    char line[LINE_MAX_LEN];
    int found = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Loại bỏ ký tự xuống dòng ở cuối nếu có */
        line[strcspn(line, "\n")] = '\0';

        /* Tạo bản sao để không làm hỏng chuỗi gốc khi dùng strtok */
        char line_copy[LINE_MAX_LEN];
        strcpy(line_copy, line);

        /* Parse từng trường dữ liệu phân tách bởi '|' */
        char *id = strtok(line_copy, "|");
        char *name = strtok(NULL, "|");
        char *class_name = strtok(NULL, "|");
        char *gpa_str = strtok(NULL, "|");

        if (id && strcmp(id, search_id) == 0) {
            found = 1;
            float gpa = gpa_str ? atof(gpa_str) : 0.0f;

            printf("\n========== SEARCH RESULT ==========\n");
            printf("  ID      : %s\n", id);
            printf("  Name    : %s\n", name ? name : "N/A");
            printf("  Class   : %s\n", class_name ? class_name : "N/A");
            printf("  GPA     : %.1f\n", gpa);
            printf("  Grade   : %s\n", get_grade_classification(gpa));
            printf("====================================\n\n");
            break;
        }
    }

    fclose(fp);

    if (found) {
        exit(0); /* Exit code 0: Tìm thấy */
    } else {
        printf("[SEARCHER] No student found with ID: %s\n\n", search_id);
        exit(1); /* Exit code 1: Không tìm thấy */
    }
}