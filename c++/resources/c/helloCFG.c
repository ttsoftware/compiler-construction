#include<stdio.h>
/**
 *
 * Play script
 *
 */
int main() {
    int* x = NULL;
    int y = 3;
    int l = 0;
    if(l == 0) {
        x = &y;
    }
    return *x;
}