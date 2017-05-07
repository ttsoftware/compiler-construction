#include<stdio.h>

/**
 *
 * Play script
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