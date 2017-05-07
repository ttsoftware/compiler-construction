#include<stdio.h>

/**
 *
 * Play script
 *
 */
int main() {
    int* x = NULL;
    int y = 3;
    int l = 1;
    if (l) {
        x = &y;
    }
    return *x;
}