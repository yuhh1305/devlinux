#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define DATA_FILE "products.dat"

typedef struct {
    int    id;
    char   name[64];
    int    quantity;
    double price;
} Product;

static void print_product(int index, const Product *p)
{
    printf("  [%d] ID: %d | %-20s | Qty: %d | Price: %.2f\n",
           index, p->id, p->name, p->quantity, p->price);
}

static void add_product(int fd)
{
    Product p;
    memset(&p, 0, sizeof(p));

    printf("  ID       : "); scanf("%d",  &p.id);
    printf("  Name     : "); scanf(" %63[^\n]", p.name);
    printf("  Quantity : "); scanf("%d",  &p.quantity);
    printf("  Price    : "); scanf("%lf", &p.price);

    lseek(fd, 0, SEEK_END);
    if (write(fd, &p, sizeof(Product)) != sizeof(Product))
        perror("write");
    else
        printf("  -> Product added.\n");
}

static int record_count(int fd)
{
    off_t size = lseek(fd, 0, SEEK_END);
    return (int)(size / sizeof(Product));
}

static void show_by_index(int fd)
{
    int idx;
    printf("  Index: "); scanf("%d", &idx);

    int n = record_count(fd);
    if (idx < 0 || idx >= n) { printf("  Index out of range (0-%d).\n", n-1); return; }

    Product p;
    off_t offset = (off_t)idx * sizeof(Product);
    lseek(fd, offset, SEEK_SET);
    if (read(fd, &p, sizeof(Product)) == (ssize_t)sizeof(Product))
        print_product(idx, &p);
    else
        perror("read");
}

static void update_quantity(int fd)
{
    int idx;
    printf("  Index: "); scanf("%d", &idx);

    int n = record_count(fd);
    if (idx < 0 || idx >= n) { printf("  Index out of range (0-%d).\n", n-1); return; }

    int new_qty;
    printf("  New quantity: "); scanf("%d", &new_qty);

    /* seek directly to the quantity field — do not rewrite the whole record */
    off_t field_offset = (off_t)idx * sizeof(Product) + offsetof(Product, quantity);
    lseek(fd, field_offset, SEEK_SET);
    if (write(fd, &new_qty, sizeof(int)) != sizeof(int))
        perror("write");
    else
        printf("  -> Quantity updated.\n");
}

static void list_all(int fd)
{
    Product p;
    int idx = 0;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &p, sizeof(Product)) == (ssize_t)sizeof(Product)) {
        print_product(idx++, &p);
    }
    if (idx == 0)
        printf("  (no records)\n");
}

int main(void)
{
    int fd = open(DATA_FILE, O_RDWR | O_CREAT, 0644);
    if (fd < 0) { perror("open"); return 1; }

    int choice;
    for (;;) {
        printf("\n=== Product Manager ===\n"
               "1. Add product\n"
               "2. Show by index\n"
               "3. Update quantity by index\n"
               "4. List all\n"
               "5. Exit\n"
               "Choice: ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: add_product(fd);     break;
            case 2: show_by_index(fd);   break;
            case 3: update_quantity(fd); break;
            case 4: list_all(fd);        break;
            case 5: close(fd); return 0;
            default: printf("  Invalid choice.\n");
        }
    }

    close(fd);
    return 0;
}
