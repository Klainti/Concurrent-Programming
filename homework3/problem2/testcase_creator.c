#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define N 1000

int main(){
  int i;

  srand(time(NULL));
  for (i=0; i<N; i++){
    printf("%d\n", rand()%3);
  }

  return(0);
}
