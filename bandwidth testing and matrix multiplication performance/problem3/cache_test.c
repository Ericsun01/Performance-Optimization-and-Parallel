#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

const int stride = 16;
int num_elements;
int num_traversals;
uint64_t *array;


double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
}

void init_array() {
  int i, j;
  uint64_t tmp;

  for (i=0; i < num_elements; i++) {
    array[i*stride] = i*stride;
  }

  i = num_elements;
  while (i > 1) {
    i--;
    j = rand() % i;
    tmp = array[i*stride];
    array[i*stride] = array[j*stride];
    array[j*stride] = tmp;
  }
}

int main(int argc, char *argv[]) {

  int i;
  uint64_t index;
  struct timespec start_time, end_time;

  num_elements = atoi(argv[1]);
  num_traversals = atoi(argv[2]);
  array = (uint64_t*)malloc(num_elements * stride * sizeof(uint64_t));

  init_array();
  
  // write only
  /* 
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  for (i=0; i < num_traversals; i++) {
    for(int j=0; j<num_elements;j++) {
      array[j*stride+1] = j;
      array[j*stride+2] = j+1;
      array[j*stride+3] = j+2;
      array[j*stride+4] = j+3;
      array[j*stride+5] = j+4;
      array[j*stride+6] = j+5;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  */

  
  // 1:1 read-to-write ratio
  /*
  int temp1 = 0;
  int temp2 = 0;
  int temp3 = 0;
  
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  for (i=0; i < num_traversals; i++) {
     for (int j=0; j < num_elements; j++) {
       temp1 = array[j*stride];
       temp2 = array[j*stride+1];
       temp3 = array[j*stride+2];
       array[j*stride+3] = j;
       array[j*stride+4] = j+1;
       array[j*stride+5] = j+2;
     }
  }
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  */

  // 2:1 read-to-write ratio
  
  int temp1 = 0;
  int temp2 = 0;
  int temp3 = 0;
  int temp4 = 0;
 
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  for (i=0; i < num_traversals; i++) {
    for(int j=0; j<num_elements;j++) {
      temp1 = array[j*stride];
      temp2 = array[j*stride+1];
      temp3 = array[j*stride+2];
      temp4 = array[j*stride+3];
      array[j*stride+4] = j;
      array[j*stride+5] = j+1;
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  
  double elapsed_ns = calc_time(start_time, end_time);
  printf("Time=%f ns\n", elapsed_ns);
  printf("GB per second=%f\n", (6 *(sizeof(uint64_t)*(uint64_t)num_elements*(uint64_t)num_traversals)/(elapsed_ns)));
  
  
  free(array);
}
