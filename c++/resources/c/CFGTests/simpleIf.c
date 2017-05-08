#include<stdio.h>
/**
 *
 * This simple if program works, but reports a failure that should have been a warning.
 * The reason is described in the report.
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    if (l) {
        x = &y;
        l = 3;
    }
    return *x;
}