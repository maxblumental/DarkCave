#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char **argv) {

  if ( argc != 3 ) {
    fprintf(stderr, "Usage: $ ./a.out N write\n");
    return 0; 
  }

  int i, j, N = atoi(argv[1]),
      write = atoi(argv[2]);
  double** a = (double**)malloc(sizeof(double*)*N);
  for (i = 0; i < N; ++i) {
    a[i] = (double*)malloc(sizeof(double)*N);
  }
  FILE *ff;

  // Initialization
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      a[i][j] = 10 * i + j;
    }
  }

  // Parallelize
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      a[i][j] = sin(0.00001 * a[i][j]);
    }
  }


  if (write) {
    ff = fopen("./data/1.txt", "w");
    for (i = 0; i < N; ++i) {
      for (j = 0; j < N; ++j) {
        fprintf(ff, "%f ", a[i][j]);
      }
      fprintf(ff, "\n");
    }
    fclose(ff);
  }
  
  for (i = 0; i < N; ++i) {
    free(a[i]);
  }
  free(a);
  
  return 0;
}
