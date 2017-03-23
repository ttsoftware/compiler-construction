#include<stdio.h>
/**
 *
 * This test should NOT fail.
 * We should handle this case correctly.
 *
 */
int main() {
    int b;
    int* k = NULL;
    k = &b;
    b = *k;
}