#include<stdio.h>

int main() {
    /*int x;
    int* a;
    while(1) {
        continue;
    } */


    int a = 3;
    int* x = &a;
    x = NULL;
    int b;
    //x=NULL;
    b=*x; // or some dereferencing of x
    return 1;
}