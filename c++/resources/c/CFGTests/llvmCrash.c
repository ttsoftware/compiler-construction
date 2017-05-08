#include<stdio.h>
/**
 * This program results in a llvm exception if line 16 is not out-commented.
 * We did not have time to find the issue, but it would be a good place
 * to continue development on this project.
 * From the stack trace, it looks like NullPointerDetector::compare(..)
 * generates the exception - so this would be a starting point when fixing
 * the bug.
 */
int main() {
    int y = 3;
    int* x = NULL;
    int l = 1;
    while (l) {
        x = &y;
        // l = l + 1; // UNCOMMENT LINE TO GENERATE EXCEPTION ON ANALYSIS.
    }
    return *x;
}
