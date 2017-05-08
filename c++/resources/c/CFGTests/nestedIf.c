#include<stdio.h>

/**
 *
 * Nested if statements work, but produce a lot of repeated output.
 * As all our tests, it suffers from the problem with overreporting
 * described in the report.
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    if (l) {
        x = &y;
        if(l) {
            x = NULL;
        }
        l = 3;
    }
    return *x;
}