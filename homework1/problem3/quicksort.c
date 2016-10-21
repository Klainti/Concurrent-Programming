/** Divide  : Partition the array A[low....high] into two sub-arrays
  *           A[low....j-1] and A[j+1...high] such that each element
  *           of A[low....j-1] is less than or equal to A[j], which
  *           in turn is is less than or equal to A[j+1...high]. Compute
  *           the index j as part of this partitioning procedure.
  * Conquer : Sort the two sub-arrays A[low....j-1] and A[j+1....high]
  *           by recursive calls to quicksort
  **/

void *quicksort(void *ptr);

#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <math.h>

volatile int *arr;
int tableSize;
volatile int nofRecursion;

int main(){

    int i,iret,arr_bound[4]; //{low,high,thread_done,depth_of_recursion}
    pthread_t main_thread;

    //The size of the table
    do{
      scanf ("%d", &tableSize);
      if (tableSize <= 0){
        printf("The size of table must not be negative or zero.\nPlease try again...");
      }
    }while (tableSize <= 0);

    //The number of the recursion
    do{
      scanf ("%d", &nofRecursion);
      if (nofRecursion <= 0){
        printf("The number of recursion must not be negative or zero.\nPlease try again...");
      }
    }while (nofRecursion <= 0);

    //The numbers to sort
    arr = (int*) calloc(tableSize, sizeof(int));
    if (arr==NULL){
        printf("Error, dynamic memory %d line\n",__LINE__);
    }

    for (i=0; i<tableSize; i++){
      scanf("%d", arr+i);
    }

    arr_bound[0]=0;
    arr_bound[1]=tableSize-1;
    arr_bound[2]=1;
    arr_bound[3]= 1; //depth of recursion

    //create thread
    iret = pthread_create(&main_thread,NULL,quicksort,(void *)arr_bound);
    if (iret){
        fprintf(stderr,"Error - pthread_create() return: %d\n",iret);
        return(EXIT_FAILURE);
    }

    while(arr_bound[2]){sched_yield();};

    printf("Sorted array\n");
    for(i = 0; i < tableSize; i++)
        printf("%d ", arr[i]);
    printf("\n");

    free((void*)arr);
    return(0);
}

void *quicksort(void *ptr){
    int pivot, i, j, temp,iret1,iret2,low,high, currentDepth;
    int left_bound_terminate[4],right_bound_terminate[4];// {low, high, thread_done, depth of the recursion}
    int *bound; //typecast *ptr
    pthread_t left_thread,right_thread;

    bound = (int *)ptr;
    low = bound[0];
    high = bound[1];
    currentDepth = bound[3];

    if(low < high) {
        pivot = low; // select a pivot element
        i = low;
        j = high;
        while(i < j) {
            // increment i till you get a number greater than the pivot element
            while(arr[i] <= arr[pivot] && i <= high)
                i++;
            // decrement j till you get a number less than the pivot element
            while(arr[j] > arr[pivot] && j >= low)
                j--;
                // if i < j swap the elements in locations i and j
            if(i < j) {
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        // when i >= j it means the j-th position is the correct position
        // of the pivot element, hence swap the pivot element with the
        // element in the j-th position
        temp = arr[j];
        arr[j] = arr[pivot];
        arr[pivot] = temp;
        // Repeat quicksort for the two sub-arrays, one to the left of j
        // and one to the right of j
        left_bound_terminate[0] = low;
        left_bound_terminate[1] = j-1;
        left_bound_terminate[2] = 1; //thread not terminated


        right_bound_terminate[0] = j+1;
        right_bound_terminate[1] = high;
        right_bound_terminate[2] = 1; //thread not terminated


        if (currentDepth+1 <= nofRecursion){
          //Increase the depth of the recursion.

          left_bound_terminate[3] = currentDepth + 1;
          right_bound_terminate[3] = currentDepth + 1;

          //Create the thread for the left subtable.
          iret1 = pthread_create(&left_thread,NULL,quicksort,(void*) left_bound_terminate);
          if (iret1){
              fprintf(stderr,"Error - pthread_create() return: %d\n",iret1);
              return(NULL);
          }

          //Create the thread for the right subtable.
          iret2 = pthread_create(&right_thread,NULL,quicksort,(void*) right_bound_terminate);
          if (iret2){
              fprintf(stderr,"Error - pthread_create() return: %d\n",iret2);
              return(NULL);
          }
        }
        else{  // if (numberOfRecursion + 1 > nofRecursion)
          //We must stop create new threads and the quicksort has to continue in one thread.
          left_bound_terminate[3] = currentDepth;
          right_bound_terminate[3] = currentDepth;
          quicksort((void*)right_bound_terminate);
          quicksort((void*)left_bound_terminate);
        }

        while(left_bound_terminate[2]){sched_yield();}; //wait until thread terminate
        while(right_bound_terminate[2]){sched_yield();}; //wait until thread terminate
    }

    bound[2]=0;
    return NULL;
}
