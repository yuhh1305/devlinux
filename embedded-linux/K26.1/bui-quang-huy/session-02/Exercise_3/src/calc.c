#include "calc.h"


double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mul(double a, double b) { return a * b; }

int divf(double a, double b, double *result) {
    if (b == 0.0) {
        return -1;
    }
    *result = a / b;
    return 0;
}