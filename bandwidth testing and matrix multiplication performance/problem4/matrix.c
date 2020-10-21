#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

const int N = 1024;
double **A;
double **B;
double **C;


double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
}

void init_matrix() {
  for(int i=0; i<N; i++) {
    for(int j=0; j<N; j++) {
      A[i][j] = i;
      B[i][j] = j;
    }
  }
}

void free_matrix() {
  for(int i=0; i<N; i++) {
    free(A[i]);
    free(B[i]);
    free(C[i]);
  }
  free(A);
  free(B);
  free(C);
}

int main(int argc, char *argv[]) {

  int i,j,k=0;
  struct timespec start_time, end_time;
  int loop_method = atoi(argv[1]);

  A = (double**)malloc(N * sizeof(double));
  B = (double**)malloc(N * sizeof(double));
  C = (double**)malloc(N * sizeof(double));

  for(int i=0; i<N; i++) {
    A[i] = (double*)malloc(sizeof(double)*N);
    B[i] = (double*)malloc(sizeof(double)*N);
    C[i] = (double*)malloc(sizeof(double)*N);
  }

  init_matrix();

  // i-j-k
  if(loop_method == 0) {
    double sum;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(i=0; i<N; i++) {
      for(j=0; j<N; j++) {
        sum=0;
        for(k=0; k<N; k++) {
          sum+=A[i][k]*B[k][j];
        }
        C[i][j]=sum;
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
  }

  // j-k-i
  if(loop_method == 1) {
    double tmp;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(j=0; j<N; j++) {
      for(k=0; k<N; k++) {
        tmp=B[k][j];
        for(i=0; i<N; i++) {
          C[i][j]+=tmp*A[i][k];
        }
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
  }

  // i-k-j
  if(loop_method == 2) {
    double tmp;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(i=0; i<N; i++) {
      for(k=0; k<N; k++) {
        tmp=A[i][k];
        for(j=0; j<N; j++) {
          C[i][j]+=tmp*B[k][j];
        }
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
  }

  //i-j-k loop tiling
  if(loop_method == 3) {
    double sum;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(i=0; i<N; i+=256) {
      for(j=0; j<N; j+=256) {
        for(int ii=i; ii < (i+256); ii++) {
          for(int jj=j; jj < (j+256); jj++) {
            sum=0;
            for(k=0; k<N; k++) {
              sum+=A[i][k]*B[k][j];
            }
            C[i][j]=sum;
          }
        }
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
  }

  
  double elapsed_ns = calc_time(start_time, end_time);
  printf("Time=%f second\n", (elapsed_ns/1000000000));
  free_matrix();
}
