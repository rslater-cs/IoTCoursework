#define TEST_MODE 0

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

#define BUFFER_SIZE 12
#define K 4
#define HIGH_ACTIVITY 500.0f
#define MEDIUM_ACTIVITY 100.0f

/*---------------------------------------------------------------------------*/
PROCESS(coursework, "Coursework");
AUTOSTART_PROCESSES(&coursework);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework, ev, data)
{
  static struct etimer timer;
  
  // create arrays for different steps of processing
  static float store_array[BUFFER_SIZE];
  static float aggregation[BUFFER_SIZE];
  static float x_bar[BUFFER_SIZE];
  static struct complex_number S[BUFFER_SIZE];
  static float R_hat[BUFFER_SIZE];
  
  // start the sensors
  start_light();
  
  // setup the fifo
  static struct fifo reads;
  init_fifo(&reads);
  reads.size = BUFFER_SIZE;
  reads.arr = store_array;
  
  static int counter = 0;

  PROCESS_BEGIN();
  
  // to gather a sample every 0.5 seconds
  etimer_set(&timer, CLOCK_CONF_SECOND/2);
  
  // Only for testing validity of calculations
  if(TEST_MODE){
    fifo_put(&reads, 1.0f);
    fifo_put(&reads, 1.3f);
    fifo_put(&reads, 2.7f);
    fifo_put(&reads, 3.0f);
    
    print_fifo(&reads);
    
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
    
    // gather k light samples
    if(counter < K || fifo_is_full(&reads) == 0){
      float light = get_light();
      printf("Reading: %i\n", (int)light);
      fifo_put(&reads, light);
      counter++;
    } else{
      printf("B = ");
      print_fifo(&reads);
      
      float u = mean(reads.arr, reads.size);
      float o2 = std2(reads.arr, reads.size, u);
      float o = sqrt(o2);
      
      printf("StdDev = %c%i.%u\n", get_sign(o), get_i(o), get_d(o));
      
      // decide on aggregation strength based on standard deviation
      unsigned short elements = BUFFER_SIZE;
      if(o < HIGH_ACTIVITY && o > MEDIUM_ACTIVITY){
        elements = 3;
      } else if(o < MEDIUM_ACTIVITY){
        elements = 1;
      }
      
      printf("Aggregation = %u-into-1\n", BUFFER_SIZE/elements);
      
      aggregate(&reads, aggregation, elements);
      
      printf("X = ");
      print_arr(aggregation, elements);
      
      // pre calculate diff between X and mean for more efficient computation
      delta_mean(&reads, x_bar, u);
      
//      printf("X-u:\n");
//      print_arr(x_bar, BUFFER_SIZE);
      
      unsigned short k = 0;
      
      // pre calculate normalised autocorrelation for more efficient computation
      while(k < BUFFER_SIZE){
        R_hat[k] = auto_correlation(x_bar, BUFFER_SIZE, k, o2);
        k++;
      }
      
      printf("Normalised Autocorrelation = \n");
      print_arr(R_hat, BUFFER_SIZE);
      
      // spectral density
      DFT(R_hat, S, BUFFER_SIZE);
      
      printf("Discrete Power Spectral Density = \n");
      print_complex_numbers(S, BUFFER_SIZE);
      
      counter = 0;
    }

    etimer_reset(&timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
