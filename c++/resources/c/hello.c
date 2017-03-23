#include<stdio.h>

int main() {
    int a;
    int b;
    b = 1;
    int* l;
    int* c;
    int* k = NULL;
    l = k;
    c = l;
    k = &b;
    b = *l;
}