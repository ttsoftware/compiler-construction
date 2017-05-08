#include<stdio.h>

/**
 * This test shows that we do not report false positives.
 * On line 17, we set x to a valid pointer, which
 * is then dereferenced on line 18. It should not generate errors or warnings.
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    int asd = y;
    if (l) {
        x = &y;
        /*l += 1;*/
    }
    x = &y;
    return *x;
}