#include <stdio.h>
#include "mathutils.h"

int main() {
    int num1, num2, fact_num;

    printf("Enter two integers for addition/subtraction (separated by space): ");
    if (scanf("%d %d", &num1, &num2) != 2) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    printf("Enter a non-negative integer for factorial: ");
    if (scanf("%d", &fact_num) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    printf("\n--- Results ---\n");
    printf("%d + %d = %d\n", num1, num2, add(num1, num2));
    printf("%d - %d = %d\n", num1, num2, subtract(num1, num2));
    
    long long fact_res = factorial(fact_num);
    if (fact_res == -1) {
        printf("Factorial of %d = Error (Negative Input)\n", fact_num);
    } else {
        printf("Factorial of %d = %lld\n", fact_num, fact_res);
    }

    return 0;
}