#include <stdio.h>

#ifndef CALC_C
#define CALC_C
#include "calculations.c" // For sqrt(), mean() and std()
#endif

#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

// returns - or no char based on sign of number
char get_sign(float num)
{
  if(num < 0.0f){
    return '-';
  }
  return '\0';
}

// get integer part of floating point number
int get_i(float num)
{
  int num_i = (int)num;
  if(num < 0.0f){
    return num_i-(2*num_i);
  }
  return num_i;
}

// rounds integer to the nearest 10
unsigned short round(uint32_t num){
  uint32_t base = num/10;
  uint32_t diff = num-(base*10);
  if(diff >= 5){
    return base+1;
  }
  return base;
}

// gets the decimal portion of a floating point to 4 d.p with rounding
unsigned short get_d(float num){
  if(num > 0.0f){
    uint32_t dec = 100000*(num-(int)num);
    return round(dec);
  }else{
    uint32_t dec = 100000*((int)num-num);
    return round(dec);
  }
}

//print contents of an array
void print_arr(float vec[], unsigned short size)
{
  unsigned short i = 1;
  
  printf("[");
  printf("%c%i.%u", get_sign(vec[0]), get_i(vec[0]), get_d(vec[0]));
  
  
  while(i < size){
    printf(", %c%i.%u", get_sign(vec[i]), get_i(vec[i]), get_d(vec[i]));
    i++;
  }
  
  printf("]\n");
}

// print contents of an array containing complex numbers
void print_complex_numbers(struct complex_number nums[], unsigned short size)
{
  unsigned short j = 1;

  printf("[%c%i.%u + %c%i.%ui", get_sign(nums[0].real), get_i(nums[0].real), get_d(nums[0].real), 
         get_sign(nums[0].i), get_i(nums[0].i), get_d(nums[0].i));
  
  while(j < size){
    float real = nums[j].real;
    float im = nums[j].i;
    printf(", %c%i.%u + %c%i.%ui", get_sign(real), get_i(real), get_d(real), get_sign(im), get_i(im), get_d(im));
    j++;
  }
  
  printf("]\n");
}

// print contents of a fifo
void print_fifo(struct fifo *instance)
{
  int i = 1;
  
  float element = fifo_get(instance, 0);
  printf("[%c%i.%u", get_sign(element), get_i(element), get_d(element));
  
  while(i < instance->size){
    element = fifo_get(instance, i);
    printf(", %c%i.%u", get_sign(element), get_i(element), get_d(element));
    i++;
  }
  printf("]\n");
}