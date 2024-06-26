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

#define BUFFER_SIZE 8
#define K 8
#define HIGH_ACTIVITY 500.0f
#define MEDIUM_ACTIVITY 100.0f

/*---------------------------------------------------------------------------*/
PROCESS(coursework, "Coursework");
AUTOSTART_PROCESSES(&coursework);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coursework, ev, data)
{
  static struct etimer timer;
  
  // array for storing contents of fifo
  static float store_array[BUFFER_SIZE];
  // array for aggregation
  static float aggregation[BUFFER_SIZE];
  // array for subtraction of mean value
  static float x_bar[BUFFER_SIZE];
  // array for DFT output
  static struct complex_number S[BUFFER_SIZE];
  // array for normalised autocorrelation
  static float R_hat[BUFFER_SIZE];
  // array for storing EMA
  static float EMA_R[BUFFER_SIZE];
  // array for storing normalisation
  static float norm[BUFFER_SIZE];
  // array for storing aggregation of norm
  static float agg[BUFFER_SIZE];
  // array for storing SAX;
  static char sax[4];
  
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
  
  // Only for testing validity of calculations using tutorial example
  if(TEST_MODE){
    fifo_put(&reads, 1.0f);
    fifo_put(&reads, 1.3f);
    fifo_put(&reads, 2.7f);
    fifo_put(&reads, 3.0f);
    
    print_fifo(&reads, 1);
    
    float u = mean(reads.arr, reads.size);
    float o2 = std2(reads.arr, reads.size, u);
    
    delta_mean(&reads, x_bar, u);
    
    printf("X-u:\n");
    print_arr(x_bar, BUFFER_SIZE, 1);
    
    unsigned short k = 0;
          
    while(k < BUFFER_SIZE){
      R_hat[k] = auto_correlation(x_bar, BUFFER_SIZE, k, o2);
      k++;
    }
    
    printf("Normalised Autocorrelation:\n");
    print_arr(R_hat, BUFFER_SIZE, 3);
    
    DFT(R_hat, S, BUFFER_SIZE);
          
    printf("DFT:\n");
    print_complex_numbers(S, BUFFER_SIZE, 3);
    
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
      print_fifo(&reads, 1);
      
      float u = mean(reads.arr, reads.size);
      float o2 = std2(reads.arr, reads.size, u);
      float o = sqrt(o2);
      
      printf("StdDev = ");
      print_float(o, 3);
      printf("\n");
      
      // decide on aggregation strength based on standard deviation
      unsigned short elements = BUFFER_SIZE;
      if(o <= HIGH_ACTIVITY && o >= MEDIUM_ACTIVITY){
        elements = 3;
      } else if(o < MEDIUM_ACTIVITY){
        elements = 1;
      }
      
      printf("Aggregation = %u-into-1\n", BUFFER_SIZE/elements);
      
      aggregate(&reads, aggregation, elements);
      
      printf("X = ");
      print_arr(aggregation, elements, 1);
      
      // pre calculate diff between X and mean for more efficient computation
      delta_mean(&reads, x_bar, u);
      
      unsigned short k = 0;
      
      // pre calculate normalised autocorrelation for more efficient computation
      while(k < BUFFER_SIZE){
        R_hat[k] = auto_correlation(x_bar, BUFFER_SIZE, k, o2);
        k++;
      }
      
      printf("Normalised Autocorrelation = \n");
      print_arr(R_hat, BUFFER_SIZE, 2);
      
      // spectral density
      DFT(R_hat, S, BUFFER_SIZE);
      
      printf("Discrete Power Spectral Density = \n");
      print_complex_numbers(S, BUFFER_SIZE, 3);
      
      
      printf("B = \n");
      print_fifo(&reads, 1);
      
      printf("Smoothing factor = ");
      print_float(SMOOTHING, 1);
      printf("\n");
      
      EMA(&reads, EMA_R);
      
      printf("EMA = \n");
      print_arr(EMA_R, BUFFER_SIZE, 3);
      
      // TODO SAX, Noramlise buffer then aggregate from 8 to 4
      // use cut off ranges to get alphabetic representation
      
      printf("X (light) = \n");
      print_fifo(&reads, 1);
      
      printf("Avg-X = ");
      print_float(u, 2);
      printf("\nVar-X = ");
      print_float(o2, 2);
      printf("\nSTD-X = ");
      print_float(o, 2);
      printf("\n");
      
      normalise(&reads, norm, u, o);
      
      printf("X normalised = \n");
      print_arr(norm, BUFFER_SIZE, 3);
      
      PAA(norm, agg, BUFFER_SIZE, 2);
      
      printf("X aggregated = \n");
      print_arr(agg, 4, 3);
      
      SAX(agg, sax, 4);
      
      printf("SAX = \n");
      print_string(sax, 4);
      
      counter = 0;
    }

    etimer_reset(&timer);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
