#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

void add_product(int fd) {
    Product p;
    printf("Nhập ID sản phẩm: "); scanf("%d", &p.id);
    getchar();
    printf("Nhập tên sản phẩm: "); fgets(p.name, sizeof(p.name), stdin);
    p.name[strcspn(p.name, "\n")] = '\0';
    printf("Nhập số lượng: "); scanf("%d", &p.quantity);
    printf("Nhập giá: "); scanf("%lf", &p.price);

    lseek(fd, 0, SEEK_END);
    ssize_t ret = write(fd, &p, sizeof(Product));
    if (ret != sizeof(Product)) {
        if (ret < 0) perror("write error");
        else fprintf(stderr, "partial write\n");
    }
}

void show_product_by_index(int fd) {
    int index;
    printf("Nhập chỉ mục (index từ 0): ");
    scanf("%d", &index);

    off_t offset = (off_t)index * sizeof(Product);

    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("Lỗi dịch chuyển con trỏ");
        return;
    }

    Product p;

    ssize_t nread = read(fd, &p, sizeof(Product));
    if (nread < 0) {
        perror("read error");
    } else if (nread != sizeof(Product)) {
        printf("Không tìm thấy sản phẩm tại index này.\n");
    } else { 
        printf("\nSản phẩm tại Index %d:\n", index);
        printf("ID: %d | Tên: %s | Số lượng: %d | Giá: %.2f\n", p.id, p.name, p.quantity, p.price); 
    }
    
}

void update_quantity_by_index(int fd) {
    int index, new_qty;
    printf("Nhập chỉ mục (index) cần cập nhật: "); scanf("%d", &index);
    printf("Nhập số lượng mới: "); scanf("%d", &new_qty);

    off_t offset = (off_t)index * sizeof(Product);
    off_t field_offset = offset + offsetof(Product, quantity);

    if (lseek(fd, field_offset, SEEK_SET) == (off_t)-1) {
        perror("Lỗi dịch chuyển tới vị trí trường dữ liệu");
        return;
    }

    ssize_t ret = write(fd, &new_qty, sizeof(int));
    if (ret < 0) {
        perror("write error");
    } else if (ret != sizeof(int)) {
        fprintf(stderr, "partial write\n");
    } else {
        printf("Cập nhật số lượng thành công tại index %d!\n", index);
    }
}

void list_all_products(int fd) {
    Product p;
    lseek(fd, 0, SEEK_SET);
    int index = 0;
    printf("\n%-5s %-5s %-30s %-10s %-10s\n", "Index", "ID", "Tên sản phẩm", "Số lượng", "Giá");
    while (read(fd, &p, sizeof(Product)) == sizeof(Product)) {
        printf("%-5d %-5d %-30s %-10d %-10.2f\n", index++, p.id, p.name, p.quantity, p.price);
    }
}

int main() {
    int fd = open("products.dat", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("Không thể mở products.dat");
        return 1;
    }

    int choice;
    while (1) {
        printf("\n--- PRODUCT MENU ---\n");
        printf("1. Add product\n");
        printf("2. Show product by index\n");
        printf("3. Update quantity by index\n");
        printf("4. List all products\n");
        printf("5. Exit\n");
        printf("Lựa chọn: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_product(fd); break;
            case 2: show_product_by_index(fd); break;
            case 3: update_quantity_by_index(fd); break;
            case 4: list_all_products(fd); break;
            case 5: close(fd); exit(0);
            default: printf("Lựa chọn sai!\n");
        }
    }
    return 0;
}