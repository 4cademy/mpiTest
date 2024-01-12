#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

#define N 18
#define LOAD_FACTOR 1000000

void additional_load() // generates load for a while
{
  int x;
  x=rand_r(&x); // confuse compiler so that it does not just remove this code
  for(int i=0;i<LOAD_FACTOR;i++)
    if (i%2==0)
      x+=i;
    else
      x-=i;
  x=rand_r(&x); // confuse compiler so that it does not just remove this code
}

int fib(int i)
{
  if ( i == 0 ) return 0;
  if ( i == 1 ) return 1;

  // additional load to make task last longer
  additional_load();

  int fib1,fib2;


  // get fibonacci recursively
#pragma omp task shared(fib1)
  fib1=fib(i-1);
#pragma omp task shared(fib2)
  fib2=fib(i-2);
#pragma omp taskwait
  return fib1+fib2;
}

int main()
{
#pragma omp parallel
    {
#pragma omp single
  printf("Fibonacci (%d)=%d\n",N,fib(N));
    }
  return 0;
}

