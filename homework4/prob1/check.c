#include <stdio.h>
#include <stdlib.h>


#include "command.h"
#include "debug.h"

int main(int argc,char *argv[]){

    int *number;

    number = (int *)malloc(sizeof(int)*3);

    number[0] = 3;
    number[1] = 2;

    divide((number+2),number[0],number[1]);

    printf(" num0: %d\n num1: %d\n num2: %d\n",number[0],number[1],number[2]);
    return(EXIT_SUCCESS);
}

