#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int task(int n, int m, int k) {
  return ((n-k) % m == 0) ? ((n-k)/m) : ((n-k)/m + 1);
}

int main(int argc, char **argv) {

  MPI_Init(NULL,NULL);

  if (argc != 3) {
    fprintf(stderr, "$ mpirun -n <n_of_processes> ./a.out N write\n");
    return 0;
  }

  int size=0, rank=0;

  MPI_Comm_size( MPI_COMM_WORLD, &size);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
 
  int i, j, N = atoi(argv[1]),
      write = atoi(argv[2]);
  int my_task = task(N-2, size, rank),
      offset = 2;
  for (i = 0; i < rank; ++i)
    offset += task(N-2, size, i);

  // Memory allocation - rank-driven
  double** a;
  if (rank==0) {
    a = (double**)malloc(sizeof(double*)*N);
    for (i = 0; i < N; ++i) {
      a[i] = (double*)malloc(sizeof(double)*N);
    }
    for (i = 0; i < N; ++i) {
      for (j = 0; j < N; ++j) {
        a[i][j] = 10 * i + j;
      }
    }
  } else {
    a = (double**)malloc(sizeof(double*)*N);
    for (i = 0; i < N; ++i) {
      a[i] = (double*)malloc(sizeof(double)*(my_task+1));
    }
    for (i = 0; i < N; ++i) {
      for (j = 0; j < my_task+1; ++j) {
        a[i][j] = 10 * i + (j+offset);
      }
    }
  }
  FILE *ff;

  double start=0, end=0;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0)
    start=MPI_Wtime();

  // Parallelize
  for (i = 1; i < N; ++i) {
    for (j = 2; j < my_task+1; ++j) {
      if (rank==size-1 && j==my_task)
        continue;
      a[i][j] = sin(0.00001 * a[i - 1][j - 2]);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  
  end=MPI_Wtime();

  if (rank>0) {
    for (i=0;i<N;i++)
      MPI_Send((void*)a[i], my_task, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  
  if (rank==0) {
    int disp=0;
    int taski=0;
    for (i=1;i<size;i++) {
      taski=task(N-2, size, i);
      for (j=0;j<N;j++){
        MPI_Recv((void*)(a[j]+disp+my_task), taski, MPI_DOUBLE, i,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      disp+=task(N-2, size, i);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0) {
    printf("%lf\n", end-start);

    if (write) {
      ff = fopen("par-5.txt", "w");
      for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
          fprintf(ff, "%f ", a[i][j]);
        }
        fprintf(ff, "\n");
      }
      fclose(ff);
    }
  }

  // Memory deallocation - rank-driven
  for (i = 0; i < N; ++i) {
    free(a[i]);
  }
  free(a);

  MPI_Finalize();

  return 0;
}
