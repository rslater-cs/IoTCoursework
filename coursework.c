#define TEST_MODE 1

#include "contiki.h"
#include <stdio.h>  // For printf()
#include <random.h> // for random_rand()
#include "calculations.c" // For sqrt(), mean() and std()
#include "light_module.c" // for get_light()
#include "logger.c" // for print_float()

#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

#define BUFFER_SIZE 12
#define K 12
#define HIGH_ACTIVITY 500.0f
#define MEDIUM_ACTIVITY 100.0f

void aggregate(struct fifo *input_vec, unsigned short output_size)
{
  unsigned short interval = input_vec->size / output_size;
  
  unsigned short i = 0;
  unsigned short j = 0;
  
  printf("[");
  
  while(i < output_size){
    j = 0;
    float total = 0.0f;
    while(j < interval){
      total += fifo_get(input_vec, i*interval+j);
      j += 1;
    }
    print_float(total/interval);
    printf(" ");
    i += 1;
  }
  
  printf("]\n");
}

void print_readings(struct fifo *readings)
{
  int i = 0;
  
  while(i < readings->size){
    print_float(readings->arr[i]);
    printf(" ");
    i++;
  }
  printf("\n");
}

void print_complex_numbers(struct complex_number nums[], unsigned short size)
{
  unsigned short j = 0;
  
  printf("[ ");
  
  while(j < size){
    print_float(nums[j].real);
    printf(" + ");
    print_float(nums[j].i);
    printf("i ");
    j++;
  }
  
  printf("]\n");
}

void print_arr(float arr[], unsigned short size)
{
  unsigned short i = 0;
  
  printf("[");
  
  while(i < size){
    print_float(arr[i]);
    printf(" ");
    i++;
  }
  printf("]\n");
}

/*---------------------------------------------------------------------------*/
PROCESS(coursework, "Coursework");
AUTOSTART_PROCESSES(&coursework);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework, ev, data)
{
  static struct etimer timer;
  
  static struct complex_number S[BUFFER_SIZE];
  static float R_hat[BUFFER_SIZE];
  
  start_light();
  
  static struct fifo reads;
  init_fifo(&reads);
  reads.size = BUFFER_SIZE;
  static float store_array[BUFFER_SIZE];
  reads.arr = store_array;
  
  static int counter = 0;

  PROCESS_BEGIN();
  etimer_set(&timer, CLOCK_CONF_SECOND/2);
  
  if(TEST_MODE){
    return;
  }

  while(1) 
  {
    PROCESS_WAIT_EVENT_UNTIL(ev=PROCESS_EVENT_TIMER); // wait for an event
    
//    printf("FIFO_INFO -> A_SIZE:%i, R_SIZE:%u, R_POS:%u, W_POS:%u, EMPTY:%u\n", reads.size, fifo_get_size(&reads), reads.r_pos, reads.w_pos, reads.empty);
    
    if(counter < K){
      float light = get_light();
      fifo_put(&reads, light);
      counter++;
    } else{
      float u = mean(reads.arr, reads.size);
      float o = std(reads.arr, reads.size, u);
      
      unsigned short elements = BUFFER_SIZE;
      if(o < HIGH_ACTIVITY && o > MEDIUM_ACTIVITY){
        elements = 4;
      } else if(o < MEDIUM_ACTIVITY){
        elements = 1;
      }
      
      aggregate(&reads, elements);
      
      printf("PRE R_hat\n");
      
      unsigned short k = 0;
      
      while(k < BUFFER_SIZE){
        R_hat[k] = auto_correlation(&reads, k, u, o);
        k++;
      }
      
      printf("PRE DFT\n");
      
      DFT(R_hat, S, BUFFER_SIZE, u, o);
      
      printf("DONE DFT\n");
      
      print_complex_numbers(S, BUFFER_SIZE);
      
      counter = 0;
    }

    etimer_reset(&timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
