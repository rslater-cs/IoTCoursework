#include <stdio.h>

#ifndef CALC_C
#define CALC_C
#include "calculations.c" // For sqrt(), mean() and std()
#endif

#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

// rounds integer to the nearest 10
unsigned short round(unsigned short num){
  unsigned short base = num/10;
  unsigned short diff = num-(base*10);
  if(diff >= 5){
    return base+1;
  }
  return base;
}

// Outputs a floating point of defined decimal places 
void print_float(float num, unsigned short decimal_places)
{
  
  int int_part = (int)num;
  
  // check if float is positive or negative and get absolute 
  // value of integer part
  char sign = '\0';
  if(num < 0.0f){
    int_part = int_part-(2*int_part);
    sign = '-';
  }
  
  // increase the factor to multiply by fot get correct decimal places
  unsigned short places = 10;
  unsigned short i = 0;
  
  while(i < decimal_places){
    places *= 10;
    i++;
  }
  
  // Get rounded decimal part
  char frac[6];
  int decimal_part = round((int)((num - (int)num + 1.0) * places));
  if(num < 0.0f){
    decimal_part = round((int)(((int)num - num + 1.0) * places));
  }
  
  // copy decimal part into array, only use part of array needed for the decimal places
  sprintf(&frac[6-decimal_places], "%d", decimal_part);
  printf("%c%d.%s", sign, int_part, &frac[6-decimal_places+1]);
}

//print contents of an array
void print_arr(float vec[], unsigned short size, unsigned short decimal_places)
{
  unsigned short i = 1;
  
  printf("[");
  print_float(vec[0], decimal_places);
  
  
  while(i < size){
    printf(", ");
    print_float(vec[i], decimal_places);
    i++;
  }
  
  printf("]\n");
}

void print_string(char str[], unsigned short size)
{
  unsigned short i = 1;
  
  printf("[%c", str[0]);
  
  while(i < size){
    printf(", %c", str[i]);
    i++;
  }
  
  printf("]\n");
}

// print contents of an array containing complex numbers
void print_complex_numbers(struct complex_number nums[], unsigned short size, unsigned short decimal_places)
{
  unsigned short j = 1;

  printf("[");
  print_float(nums[0].real, decimal_places);
  printf(" + ");
  print_float(nums[0].i, decimal_places);
  printf("i");
  
  while(j < size){
    printf(", ");
    print_float(nums[j].real, decimal_places);
    printf(" + ");
    print_float(nums[j].i, decimal_places);
    printf("i");
    j++;
  }
  
  printf("]\n");
}

// print contents of a fifo
void print_fifo(struct fifo *instance, unsigned short decimal_places)
{
  int i = 1;
  
  float element = fifo_get(instance, 0);
  printf("[");
  print_float(element, decimal_places);
  
  while(i < instance->size){
    element = fifo_get(instance, i);
    printf(", ");
    print_float(element, decimal_places);
    i++;
  }
  printf("]\n");
}