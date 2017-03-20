#include<stdio.h>

int main() {
    char a = NULL;
    char *b = NULL;
    //char *l = b;
    char x = 1;
    b = &x;
    char c = *b + 0x01;

    return 1;
}