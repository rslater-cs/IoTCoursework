#include <stdio.h>

void print_float(float num)
{
  char fractional_part[10];
  sprintf(fractional_part, "%d", (int)((num - (int)num + 1.0) * 1000000));
  printf("%d.%s", (int)num, &fractional_part[1]);
}

void print_aggregate(float vec[], unsigned short size)
{
  unsigned short i = 0;
  
  printf("[");
  
  while(i < size){
    print_float(vec[i]);
    printf(" ");
  }
  
  printf("]\n");
}