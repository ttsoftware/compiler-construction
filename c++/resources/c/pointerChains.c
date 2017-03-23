#include<stdio.h>
/**
 *
 * This test should NOT fail.
 * We should handle this case correctly, because it does not rely on updating references to work.
 *
 */
int main() {
    int b = 2;
    int* l;
    int* c;
    int* k = &b;
    l = k;
    c = l;
    b = *l;
}