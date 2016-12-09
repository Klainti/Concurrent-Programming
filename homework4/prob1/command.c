#include <stdio.h>
#include <stdlib.h>
#include "debug.h"

void add(int *variable,int var1,int var2){
    *variable = var1 + var2;
}

void sub(int *variable,int var1,int var2){
    *variable = var1 - var2;
}

void mul(int *variable,int var1,int var2){
    *variable = var1*var2;
}

void divide(int *variable,int var1,int var2){
    if (var2==0){
        debug_e("Try to divide by zero!");
        exit(EXIT_FAILURE);
    }
    *variable = var1/var2;
}

void mod(int *variable,int var1,int var2){
    *variable = var1%var2;
}

