#include <stdio.h>
#include "calc.h"
#include "logger.h"

int main() {
    double a = 12.5, b = 4.0;
    double zero = 0.0;
    double res;
    char log_buffer[256];

    log_info("Application started.");

    // Addition
    res = add(a, b);
    snprintf(log_buffer, sizeof(log_buffer), "Performed Add: %.2f + %.2f = %.2f", a, b, res);
    log_info(log_buffer);

    // Subtraction
    res = sub(a, b);
    snprintf(log_buffer, sizeof(log_buffer), "Performed Sub: %.2f - %.2f = %.2f", a, b, res);
    log_info(log_buffer);

    // Division (Success Case)
    if (divf(a, b, &res) == 0) {
        snprintf(log_buffer, sizeof(log_buffer), "Performed Div: %.2f / %.2f = %.2f", a, b, res);
        log_info(log_buffer);
    }

    // Division by Zero (Error Case)
    if (divf(a, zero, &res) != 0) {
        log_error("Division by zero attempted!");
    }

    log_info("Application finished smoothly.");

    // Terminal summary layout
    printf("--- Execution Summary ---\n");
    printf("Add (%.2f + %.2f) = %.2f\n", a, b, add(a, b));
    printf("Sub (%.2f - %.2f) = %.2f\n", a, b, sub(a, b));
    printf("Mul (%.2f * %.2f) = %.2f\n", a, b, mul(a, b));
    printf("Check 'app.log' for deep execution context history.\n");

    return 0;
}