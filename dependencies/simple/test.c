#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int task(int n, int m, int k) {
  return ((n-k) % m == 0) ? ((n-k)/m) : ((n-k)/m + 1);
}

typedef struct arguments {
  double** a;
  int Ni;
  int Nj;
} arguments;

void foo(arguments* args) {
  int i,j;
  double** d = args->a;
  for (i = 0; i < args->Ni; ++i) {
    for (j = 0; j < args->Nj; ++j) {
      d[i][j] = sin(0.00001 * d[i][j]);
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char **argv) {

  if ( argc != 4 ) {
    fprintf(stderr, "Usage: $ ./a.out n_of_processes N write\n");
    return 0;
  }

  int i, j, np = atoi(argv[1]),
      N = atoi(argv[2]), write = atoi(argv[3]);
  FILE *ff;

  pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*np);

  //Prepare the tasks
  arguments* tasks = (arguments*)malloc(sizeof(arguments)*np);
  for (i=0;i<np;i++){
    tasks[i].Ni = task(N, np, i);
    tasks[i].Nj = N;
    tasks[i].a = (double**)malloc(sizeof(double*)*tasks[i].Ni);
    for (j=0;j<tasks[i].Ni;j++) {
      tasks[i].a[j] = (double*)malloc(sizeof(double)*tasks[i].Nj);
    }
  }

  // Initialization
  int k=0, buf=0;
  for (k = 0; k < np; ++k) {
    for (i = 0; i < tasks[k].Ni; ++i) {
      for (j = 0; j < tasks[k].Nj; ++j) {
        tasks[k].a[i][j] = 10 * (i+buf) + j;
      }
    }
    buf += tasks[k].Ni;
  }

  clock_t t1=clock();

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

  clock_t t2=clock();
  printf("%.3f\n", (float)(t2-t1)/CLOCKS_PER_SEC);

  if (write) {
    ff = fopen("par-1.txt", "w");
    for (i = 0; i < np; ++i) {
      for (j = 0; j < tasks[i].Ni; ++j) {
        for (k = 0; k < tasks[i].Nj; ++k) {
          fprintf(ff, "%f ", tasks[i].a[j][k]);
        }
        fprintf(ff, "\n");
      }
    }
    fclose(ff);
  }

  free(threads);
  for (i=0;i<np;i++){
    for (j=0;j<tasks[i].Ni;j++) {
      free(tasks[i].a[j]);
    }
    free(tasks[i].a);
  }
  free(tasks);
  return 0;
}
