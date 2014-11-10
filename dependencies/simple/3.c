#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {

  if (argc != 3) {
    printf("Usage: $ ./a.out N write\n");
    return 0;
  }

  int i, j;
  FILE *ff;
  int N = atoi(argv[1]),
      write = atoi(argv[2]);
  double** a = (double**)malloc(sizeof(double*)*N);
  for (i = 0; i < N; ++i) {
    a[i] = (double*)malloc(sizeof(double)*N);
  }

  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      a[i][j] = 10 * i + j;
    }
  }


  clock_t t1 = clock();

  for (i = 0; i < N - 1; ++i) {
    for (j = 1; j < N; ++j) {
      a[i][j] = sin(0.00001 * a[i+1][j-1]);
    }
  }

  clock_t t2 = clock();
  printf("%f\n", (float)(t2-t1)/CLOCKS_PER_SEC);

  if (write) {
    ff = fopen("3.txt", "w");
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
