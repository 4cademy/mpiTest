/* 
 * This file is part of the KdpP lecture.
 * Copyright (c) 2021 TU Dresden.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#define LOOP_ITERATIONS 1000

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <unistd.h>

unsigned int* rand_var;

void main()
{
  int thread_executing_the_iteration[LOOP_ITERATIONS];

  rand_var = malloc(sizeof(unsigned int)*omp_get_max_threads());
  for (int i=0;i<omp_get_max_threads();i++)
    rand_var[i]=i;

  #pragma omp parallel for schedule (runtime)
  for (int i=0;i<LOOP_ITERATIONS;i++)
  {
    int my_thread=omp_get_thread_num();
    rand_var[my_thread]=rand_r(&rand_var[my_thread]);
    usleep(rand_var[my_thread]%1000);
    thread_executing_the_iteration[i]=my_thread;
  }
  printf ("Loop itration, thread\n");
  for (int i=0;i<LOOP_ITERATIONS;i++)
  {
     printf("%d, %d\n",i,thread_executing_the_iteration[i]);
  }
}
