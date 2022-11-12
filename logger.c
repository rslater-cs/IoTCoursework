#include <stdio.h>

#ifndef CALC_C
#define CALC_C
#include "calculations.c" // For sqrt(), mean() and std()
#endif

#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

char get_sign(float num)
{
  if(num < 0.0f){
    return '-';
  }
  return '\0';
}

int get_i(float num)
{
  int num_i = (int)num;
  if(num < 0.0f){
    return num_i-(2*num_i);
  }
  return num_i;
}

unsigned short round(uint32_t num){
  uint32_t base = num/10;
  uint32_t diff = num-(base*10);
  if(diff >= 5){
    return base+1;
  }
  return base;
}

unsigned short get_d(float num){
  if(num > 0.0f){
    uint32_t dec = 100000*(num-(int)num);
    return round(dec);
  }else{
    uint32_t dec = 100000*((int)num-num);
    return round(dec);
  }
}

void print_arr(float vec[], unsigned short size)
{
  unsigned short i = 0;
  
  printf("[");
  
  while(i < size){
    printf("%c%i.%u ", get_sign(vec[i]), get_i(vec[i]), get_d(vec[i]));
    i++;
  }
  
  printf("]\n");
}

void print_complex_numbers(struct complex_number nums[], unsigned short size)
{
  unsigned short j = 0;
  
  printf("[ ");
  
  while(j < size){
    float real = nums[j].real;
    float im = nums[j].i;
    printf("%c%i.%u + %c%i.%ui ", get_sign(real), get_i(real), get_d(real), get_sign(im), get_i(im), get_d(im));
    j++;
  }
  
  printf("]\n");
}

void print_fifo(struct fifo *instance)
{
  int i = 0;
  
  printf("[ ");
  
  while(i < instance->size){
    float num = instance->arr[i];
    printf("%c%i.%u ", get_sign(num), get_i(num), get_d(num));
    i++;
  }
  printf("]\n");
}