#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  
  if (argc != 4) {
    printf("Usage: $ ./a.out n_of_processes N write\n");
    return 0;
  }

  int i, j;
  FILE *ff;
  int np = atoi(argv[1]),
      N = atoi(argv[2]),
      write = atoi(argv[3]);
  double** a = (double**)malloc(sizeof(double*)*N);
  double** b = (double**)malloc(sizeof(double*)*N);
  for (i = 0; i < N; ++i) {
    a[i] = (double*)malloc(sizeof(double)*N);
    b[i] = (double*)malloc(sizeof(double)*N);
  }

  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      a[i][j] = 10 * i + j;
      b[i][j] = 10 * i + j;
    }
  }


  clock_t t1 = clock();

  #pragma omp parallel for private(i,j)
  for (i = 0; i < N - 1; ++i) {
    for (j = 1; j < N; ++j) {
      a[i][j] = sin(0.00001 * b[i+1][j-1]);
    }
  }

  clock_t t2 = clock();
  printf("%f\n", (float)(t2-t1)/CLOCKS_PER_SEC);
 
  if (write) {
    ff = fopen("par-3.txt", "w");
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
    free(b[i]);
  }
  free(a);
  free(b);
  
  return 0;
}
