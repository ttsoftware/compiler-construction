#include<stdio.h>
/**
 *
 * This test should NOT fail.
 * We should handle this case correctly, because it does not rely on updating references to work.
 *
 */
int main() {
    int a = 2;
    int b = 2;
    int c = 2;
    int* d;
    int* e;
    int* f = NULL;
    d = f;
    e = f;


    a = *d;
    b = *e;
    c = *f;

}