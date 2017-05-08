#include<stdio.h>

/**
 *
 * This test is expected to give a warning. When we dereference
 * l on line 22, its value can either come from line 18, or line 20.
 * In this case, we should set the value of l to MAYBE_NULL.
 *
 *
 *
 *
 */
int main() {
    int b = 2;
    int* l;
    int* c;
    int* k = &b;
    l = k;
    while (b) {
        l = c;
    }
    b = *l;
}