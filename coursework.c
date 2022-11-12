#define TEST_MODE 1

#include "contiki.h"
#include <stdio.h>  // For printf()
#include <random.h> // for random_rand()
#include "light_module.c" // for get_light()
#include "logger.c" // for print_float()

#ifndef FIFO_C
#define FIFO_C
#include "fifo.c" // data structure to hold sensor readings
#endif

#ifndef CALC_C
#define CALC_C
#include "calculations.c" // For sqrt(), mean() and std()
#endif

#define BUFFER_SIZE 4
#define K 12
#define HIGH_ACTIVITY 500.0f
#define MEDIUM_ACTIVITY 100.0f

/*---------------------------------------------------------------------------*/
PROCESS(coursework, "Coursework");
AUTOSTART_PROCESSES(&coursework);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework, ev, data)
{
  static struct etimer timer;
  
  static float store_array[BUFFER_SIZE];
  static float aggregation[BUFFER_SIZE];
  static float x_bar[BUFFER_SIZE];
  static struct complex_number S[BUFFER_SIZE];
  static float R_hat[BUFFER_SIZE];
  
  start_light();
  
  static struct fifo reads;
  init_fifo(&reads);
  reads.size = BUFFER_SIZE;
  reads.arr = store_array;
  
  static int counter = 0;

  PROCESS_BEGIN();
  etimer_set(&timer, CLOCK_CONF_SECOND/2);
  
  if(TEST_MODE){
    fifo_put(&reads, 1.0f);
    fifo_put(&reads, 1.3f);
    fifo_put(&reads, 2.7f);
    fifo_put(&reads, 3.0f);
    
    float u = mean(reads.arr, reads.size);
    float o2 = std2(reads.arr, reads.size, u);
    
    delta_mean(&reads, x_bar, u);
    
    printf("X-u:\n");
    print_arr(x_bar, BUFFER_SIZE);
    
    unsigned short k = 0;
          
    while(k < BUFFER_SIZE){
      R_hat[k] = auto_correlation(x_bar, BUFFER_SIZE, k, o2);
      k++;
    }
    
    printf("Normalised Autocorrelation:\n");
    print_arr(R_hat, BUFFER_SIZE);
    
    DFT(R_hat, S, BUFFER_SIZE);
          
    printf("DFT:\n");
    print_complex_numbers(S, BUFFER_SIZE);
    
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
      float o2 = std2(reads.arr, reads.size, u);
      float o = sqrt(o2);
      
      unsigned short elements = BUFFER_SIZE;
      if(o < HIGH_ACTIVITY && o > MEDIUM_ACTIVITY){
        elements = 4;
      } else if(o < MEDIUM_ACTIVITY){
        elements = 1;
      }
      
      aggregate(&reads, aggregation, elements);
      
      print_arr(aggregation, elements);
      
      delta_mean(&reads, x_bar, u);
      
      print_arr(x_bar, BUFFER_SIZE);
      
      printf("PRE R_hat\n");
      
      unsigned short k = 0;
      
      while(k < BUFFER_SIZE){
        R_hat[k] = auto_correlation(x_bar, BUFFER_SIZE, k, o2);
        k++;
      }
      
      print_arr(R_hat, BUFFER_SIZE);
      
      printf("PRE DFT\n");
      
      DFT(R_hat, S, BUFFER_SIZE);
      
      printf("DONE DFT\n");
      
      print_complex_numbers(S, BUFFER_SIZE);
      
      counter = 0;
    }

    etimer_reset(&timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
