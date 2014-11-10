#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int task(int n, int m, int k) {
  return ((n-k) % m == 0) ? ((n-k)/m) : ((n-k)/m + 1);
}

typedef struct {
  double** a;
  int start;
  int end;
  int N;
} arguments;

void foo(arguments* args) {
  int i,j;
  for (i = args->start; i < args->end; ++i) {
    for (j = 0; j < args->N; ++j) {
      args->a[i][j] = sin(0.00001 * args->a[i][j]);
    }
  }
}

int main(int argc, char **argv) {

  if ( argc != 4 ) {
    fprintf(stderr, "Usage: $ ./a.out n_of_processes N write\n");
    return 0; 
  }

  int i, j, np = atoi(argv[1]),
      N = atoi(argv[2]), write = atoi(argv[3]);
  FILE *ff;
  double** a = (double**)malloc(sizeof(double*)*N);
  for (i = 0; i < N; ++i) {
    a[i] = (double*)malloc(sizeof(double)*N);
  }
  pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*np);

  // Initialization
  for (i = 0; i < N; ++i) {
    for (j = 0; j < N; ++j) {
      a[i][j] = 10 * i + j;
    }
  }

  //Prepare the tasks
  int buf = 0;
  arguments* tasks = (arguments*)malloc(sizeof(arguments)*np);
  for (i = 0; i < np; ++i) {
    tasks[i].a = a;
    tasks[i].start = buf;
    tasks[i].end = buf+task(N, np, i);
    tasks[i].N = N; 
    buf += task(N, np, i);
  }

  struct timespec start, finish;
  double elapsed;

  clock_gettime(CLOCK_MONOTONIC, &start);

  // Parallelize
  for (i = 0; i < np; ++i) {
    if( pthread_create( threads+i, NULL,
                        (void*(*)(void*))foo,
                        (void*)(tasks+i)) ) {
      fprintf(stderr, "Failed to create a thread!\n");
      exit(0);
    }
  }

  for (i = 0; i < np; ++i) {
    if( pthread_join( threads[i], NULL ) ) {
      fprintf(stderr, "Thread %d failed to complete!\n", i);
      exit(0);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &finish);

  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

  printf("%lf\n", elapsed);

  if (write) {
    ff = fopen("par-1.txt", "w");
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

  free(threads);
  free(tasks);
  return 0;
}
