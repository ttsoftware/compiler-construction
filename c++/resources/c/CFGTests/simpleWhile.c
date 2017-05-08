#include<stdio.h>

/**
 *
 * This test is important, as it helps us verify that
 * our worklist algorithm terminates when there is
 * cycles in the CFG. It's a simple loop that should produce warnings.
 *
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    int asd = y;
    while (l != asd) {
        x = &y;
    }
    return *x;
}