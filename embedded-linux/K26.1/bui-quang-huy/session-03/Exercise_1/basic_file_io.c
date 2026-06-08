#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NAME_LEN 64

typedef struct {
    uint32_t id;
    char    name[NAME_LEN];
    uint32_t age;
    float   gpa;
} Student;

void add_student(int fd) {
    Student s;
    printf("Nhập ID: ");
    scanf("%d", &s.id);
    getchar(); 
    
    printf("Nhập tên: ");
    fgets(s.name, sizeof(s.name), stdin);
    s.name[strcspn(s.name, "\n")] = '\0'; 
    
    printf("Nhập tuổi: ");
    scanf("%d", &s.age);
    printf("Nhập GPA: ");
    scanf("%f", &s.gpa);

    lseek(fd, 0, SEEK_END);
    
    ssize_t written = 0, to_write = sizeof(Student);
    while (written < to_write) {
        ssize_t ret = write(fd, (char*)&s + written, to_write - written);
        if (ret < 0) {
            perror("write error");
            return;
        }
        written += ret;
    }

    int ret = write(fd, &s, sizeof(Student));
    
    if (ret != sizeof(Student)) {
        perror("Lỗi khi ghi dữ liệu sinh viên");
    } else {
        printf("Thêm sinh viên thành công!\n");
    }
}

void list_students(int fd) {
    Student s;

    lseek(fd, 0, SEEK_SET);
    
    printf("\n=== Danh sách sinh viên ===\n");
    printf("%-5s %-30s %-5s %-5s\n", "ID", "Họ và Tên", "Tuổi", "GPA");
    
    ssize_t nread;
    while ((nread = read(fd, &s, sizeof(Student))) > 0) {
        if (nread != sizeof(Student)) {
            fprintf(stderr, "partial read\n");
            break;
        }
        printf("%-5d %-30s %-5d %-5.2f\n", s.id, s.name, s.age, s.gpa);
    }
}

void find_student(int fd) {
    int target_id;
    Student s;
    int found = 0;
    
    printf("Nhập ID sinh viên cần tìm: ");
    scanf("%d", &target_id);
    
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(Student)) == sizeof(Student)) {
        if (s.id == target_id) {
            printf("\nSinh viên tìm thấy:\n");
            printf("ID: %d\nTên: %s\nTuổi: %d\nGPA: %.2f\n", s.id, s.name, s.age, s.gpa);
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("Không tìm thấy sinh viên có ID %d.\n", target_id);
    }
}

int main() {

    int fd = open("students.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Không thể mở tệp students.dat");
        return 1;
    }

    int choice;
    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Add student\n");
        printf("2. List all students\n");
        printf("3. Find student by ID\n");
        printf("4. Exit\n");
        printf("Lựa chọn của bạn: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_student(fd); break;
            case 2: list_students(fd); break;
            case 3: find_student(fd); break;
            case 4: 
                close(fd); 
                printf("Tạm biệt!\n");
                exit(0);
            default: printf("Lựa chọn không hợp lệ!\n");
        }
    }
    return 0;
}