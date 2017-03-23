#include<stdio.h>
/**
 *
 * This test should NOT fail. Until we update references in our nullPointerMap, it will fail however,
 * and say that b = *l generates a nullpointer exception.
 *
 * FALSE POSITIVE TEST.
 *
 */
int main() {
    int b = 2;
    int* l;
    int* c;
    int* k = NULL;
    l = k;
    c = l;

    k = &b;
    b = *l;
}