#include<stdio.h>

/**
 *
 * This test verifies that nothing is reported when there
 * are no null pointer exceptions.
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    x = &y;
    return *x;
}