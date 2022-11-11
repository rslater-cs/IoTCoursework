#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

#define PI 3.14159f

struct complex_number
{
  float real;
  float i;
};

float sqrt(float num)
{
  float r = num;
  float bound = 0.01f*r;
  
  float prev = r;
  float pred = r;
  float estimate = pred*pred;
  
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

float std(float vec[], unsigned short size, float mn)
{
  float total = 0.0f;
  
  unsigned short i = 0;
  
  while(i < size){
    float diff = vec[i]-mn;
    total += diff*diff;
    i++;
  }
  
  total /= size;
  
  total = sqrt(total);
  
  return total;
}

float auto_correlation(struct fifo *instance, unsigned short k, float u, float o)
{
  unsigned short iterations = instance->size-k;
  unsigned short i = 0; 
  
  float total = 0.0f;
  
  while(i < iterations){
    total += (fifo_get(instance, i)-u)*(fifo_get(instance, i+k)-u);
    i++;
  }
  
  total /= (o*o*iterations);
  
  return total;
}

float fmod(float num1, float num2)
{
  float res = num1/num2;
  int res_i = (int)res;
  
  float remainder = res - (float)res_i;
  remainder = remainder*num2;
  
  return remainder;
}

// estimates sine function in bounds [0, pi]
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

// implements cosine using estimations with transformations, works with bounds [0, 2*pi]
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

struct complex_number DFT_L(float R_hat[], unsigned short size, unsigned short l, float u, float o)
{
  unsigned short k = 0;
  
  struct complex_number result;
  result.real = 0.0f;
  result.i = 0.0f;
  
  while(k < size){
    float power = (float)(2*k*l)/(float)size;
    power = fmod(power, 2.0f);
    power = power * PI;
    float real = cos(power);
    float imaginary = sin(power);
    float correlation = R_hat[k];
    result.real += real*correlation;
    result.i += imaginary*correlation;
    k++;
  }
  
  return result;
}

void DFT(float R_hat[], struct complex_number results[], unsigned short size, float u, float o)
{
  
  unsigned short l = 0;
  
  while(l < size){
    results[l] = DFT_L(R_hat, size, l, u, o);
    l++;
  }
}