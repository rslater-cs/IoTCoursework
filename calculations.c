#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif
#include <stdio.h>

#define PI 3.14159f

struct complex_number
{
  float real;
  float i;
};

// Appoximation of square root function
float sqrt(float num)
{
  float r = num;
  float bound = 0.01f*r;
  
  float prev = r;
  float pred = r;
  float estimate = pred*pred;
  
  // Limit iterations to avoid getting trapped in loop
  unsigned short max_iterations = 20;
  unsigned short i = 0;
  
  while(((-bound > (estimate-r)) || ((estimate-r) > bound)) && i < max_iterations)
  {
    pred = (prev+(r/prev))/2.0f;
    prev = pred;
    estimate = pred*pred;
    i++;
  }
  
  return pred;
}

// returns the mean value of an array
float mean(float vec[], unsigned short size)
{
  float total = 0.0f;
  
  unsigned short i = 0;
  
  while(i < size){
    total += vec[i];
    i++;
  }
  
  total /= size;
  
  return total;
}

// returns the standard deviation squared of an array
float std2(float vec[], unsigned short size, float mn)
{
  float total = 0.0f;
  
  unsigned short i = 0;
  
  while(i < size){
    float diff = vec[i]-mn;
    total += diff*diff;
    i++;
  }
  
  total /= size;
  
  return total;
}

// Aggregates contents of a fifo into a set output size
void aggregate(struct fifo *instance, float result[], unsigned short output_size)
{
  unsigned short interval = instance->size / output_size;
    
  unsigned short i = 0;
  unsigned short j = 0;
  
  while(i < output_size){
    j = 0;
    result[i] = 0.0f;
    while(j < interval){
      result[i] += fifo_get(instance, i*interval+j);
      j++;
    }
    result[i] /= interval;
    i++;
  }
}

// Calculate the different between the elements of an array and the mean of the array
void delta_mean(struct fifo *instance, float result[], float u)
{
  unsigned short i = 0;
  
  while(i < instance->size){
    result[i] = fifo_get(instance, i)-u;
    i++;
  }
}

// Calculate the normalised autocorrelation of an array
float auto_correlation(float mean_diff[], unsigned short size, unsigned short k, float o2)
{
  unsigned short iterations = size-k;
  unsigned short i = 0; 
  
  float total = 0.0f;
  
  while(i < iterations){
    total += mean_diff[i]*mean_diff[i+k];
    i++;
  }
  
  total /= (o2*iterations);
  
  return total;
}

// Implementation of % function with floating point arguments
float fmod(float num1, float num2)
{
  float res = num1/num2;
  int res_i = (int)res;
  
  float remainder = res - (float)res_i;
  remainder = remainder*num2;
  
  return remainder;
}

// estimates sine function in bounds [0, pi] (using radians) with Bhaskara I's formula
float estim_sin(float radians)
{
  float numerator = 16*radians*(PI-radians);
  float denominator = (5*PI*PI)-(4*radians*(PI-radians));
  float result = numerator/denominator;
  return result;
}

// implements sine using estimation with transformations, works with bounds [0, 2*pi]
float sin(float radians)
{
  if(radians <= PI){
    return estim_sin(radians);
  } else{
    return -estim_sin(radians-PI);
  }
}

// implements cosine using estimation with transformations, works with bounds [0, 2*pi]
float cos(float radians)
{
  if(radians <= PI/2){
    return estim_sin(radians+(PI/2));
  } else if(radians > PI/2 && radians <= (3*PI)/2){
    return -estim_sin(radians-(PI/2));
  } else{
    return estim_sin(radians-(3*PI)/2);
  }
}

// Calculates spectral density S for given value of l
struct complex_number DFT_L(float R_hat[], unsigned short size, unsigned short l)
{
  unsigned short k = 0;
  
  struct complex_number result;
  result.real = 0.0f;
  result.i = 0.0f;
  
  while(k < size){
    float numerator = (float)(2*k*l);
    float denominator = (float)size;
    float power = numerator/denominator;
    power = fmod(power, 2.0f);
    power = power * PI;
    float real = cos(power);
    float imaginary = sin(power);
    float correlation = R_hat[k];
    result.real += real*correlation;
    result.i += imaginary*correlation;
    k++;
  }
  
  result.real /= size;
  result.i /= size;
  
  return result;
}

// Calculates spectral desnity S for all values of l
void DFT(float R_hat[], struct complex_number results[], unsigned short size)
{
  
  unsigned short l = 0;
  
  while(l < size){
    results[l] = DFT_L(R_hat, size, l);
    l++;
  }
}