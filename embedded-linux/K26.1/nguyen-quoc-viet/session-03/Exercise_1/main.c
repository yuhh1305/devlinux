#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define DATA_FILE "students.dat"

typedef struct {
    int   id;
    char  name[64];
    int   age;
    float gpa;
} Student;

static void print_student(const Student *s)
{
    printf("  ID: %d | Name: %-20s | Age: %d | GPA: %.2f\n",
           s->id, s->name, s->age, s->gpa);
}

static void add_student(int fd)
{
    Student s;
    memset(&s, 0, sizeof(s));

    printf("  ID   : "); scanf("%d", &s.id);
    printf("  Name : "); scanf(" %63[^\n]", s.name);
    printf("  Age  : "); scanf("%d", &s.age);
    printf("  GPA  : "); scanf("%f", &s.gpa);

    lseek(fd, 0, SEEK_END);
    if (write(fd, &s, sizeof(Student)) != sizeof(Student))
        perror("write");
    else
        printf("  -> Student added.\n");
}

static void list_students(int fd)
{
    Student s;
    int count = 0;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(Student)) == (ssize_t)sizeof(Student)) {
        print_student(&s);
        count++;
    }
    if (count == 0)
        printf("  (no records)\n");
}

static void find_student(int fd)
{
    int target;
    printf("  ID to find: "); scanf("%d", &target);

    Student s;
    lseek(fd, 0, SEEK_SET);
    while (read(fd, &s, sizeof(Student)) == (ssize_t)sizeof(Student)) {
        if (s.id == target) {
            print_student(&s);
            return;
        }
    }
    printf("  Not found: ID %d\n", target);
}

int main(void)
{
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    int choice;
    for (;;) {
        printf("\n=== Student Manager ===\n"
               "1. Add student\n"
               "2. List all\n"
               "3. Find by ID\n"
               "4. Exit\n"
               "Choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: add_student(fd);  break;
            case 2: list_students(fd); break;
            case 3: find_student(fd);  break;
            case 4: close(fd); return 0;
            default: printf("  Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}
