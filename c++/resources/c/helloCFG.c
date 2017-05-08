#include<stdio.h>

/**
 *
 * Play script
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    if (l) {
        x = &y;
    }
    return *x;
}