struct fifo {
  unsigned short r_pos;
  unsigned short w_pos;
  unsigned short size;
  unsigned short empty;
  unsigned short initialised;
  float *arr;
};

// setup starting values of a fifo, can only be called once per fifo
void init_fifo(struct fifo *instance)
{
  if(instance->initialised != 1){
    instance->initialised = 1;
    instance->r_pos = 0;
    instance->w_pos = 0;
    instance->empty = 1;
  }
}

// check if the fifo is empty
unsigned short fifo_is_empty(struct fifo *instance)
{
  return instance->empty;
}

// check if the fifo is full
unsigned short fifo_is_full(struct fifo *instance)
{
  if(fifo_is_empty(instance) == 0 && instance->r_pos == instance->w_pos){
    return 1;
  }
  return 0;
}

// put a element at the end of the fifo if fifo is full then start overwriting
unsigned short fifo_put(struct fifo *instance, float item)
{
  if(fifo_is_full(instance)){
    instance->r_pos += 1;
    instance->r_pos %= instance->size;
  }

  instance->empty = 0;
  
  instance->arr[instance->w_pos] = item;
  instance->w_pos += 1;
  
  instance->w_pos %= instance->size;
  
  return 1;
}

// get an element from a relative position of the fifo
float fifo_get(struct fifo *instance, unsigned short index)
{
  unsigned short rel_index = (index+instance->r_pos) % instance->size;
  return instance->arr[rel_index];
}