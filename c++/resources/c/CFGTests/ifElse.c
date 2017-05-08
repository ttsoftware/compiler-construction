#include<stdio.h>
/**
 *
 * Our analysis correctly reports a warning when dereferencing x,
 * in the end of the program. Surprisingly, no failure is reported -
 * Which there shouldn't be, but this was observed in many of
 * our other tests.
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    if (l) {
        x = &y;
        l = 3;
    } else {
        x = NULL;
    }
    return *x;
}