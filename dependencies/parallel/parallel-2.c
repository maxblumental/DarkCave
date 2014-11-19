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
 
  int task = task(N, size, rank),
      offset = 0;
  for (i = 0; i < rank; ++i)
    offset += task(N, size, i);

  // Memory allocation - rank-driven
  int i, j, N = atoi(argv[1]),
      write = atoi(argv[2]);
  double** a;
  double** b;
  if (rank==0) {
    a = (double**)malloc(sizeof(double*)*N);
    b = (double**)malloc(sizeof(double*)*N);
    for (i = 0; i < N; ++i) {
      a[i] = (double*)malloc(sizeof(double)*N);
      b[i] = (double*)malloc(sizeof(double)*N);
    }
    for (i = 0; i < N; ++i) {
      for (j = 0; j < task+1; ++j) {
        a[i][j] = 10 * i + j;
        b[i][j] = 10 * i + j;
      }
    }
  } else if (rank==size-1) {
    a = (double**)malloc(sizeof(double*)*N);
    b = (double**)malloc(sizeof(double*)*N);
    for (i = 0; i < N; ++i) {
      a[i] = (double*)malloc(sizeof(double)*task);
      b[i] = (double*)malloc(sizeof(double)*task);
    }
    for (i = 0; i < N; ++i) {
      for (j = 0; j < task; ++j) {
        a[i][j] = 10 * i + (j+offset);
        b[i][j] = 10 * i + (j+offset);
      }
    }
  } else {
    a = (double**)malloc(sizeof(double*)*N);
    b = (double**)malloc(sizeof(double*)*N);
    for (i = 0; i < N; ++i) {
      a[i] = (double*)malloc(sizeof(double)*(task+1));
      b[i] = (double*)malloc(sizeof(double)*(task+1));
    }
    for (i = 0; i < N; ++i) {
      for (j = 0; j < task+1; ++j) {
        a[i][j] = 10 * i + (j+offset);
        b[i][j] = 10 * i + (j+offset);
      }
    }
  }
  FILE *ff;

  double start=0, end=0;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0)
    start=MPI_Wtime();

  // Parallelize - uses it's own limits
  for (i = 1; i < N; ++i) {
    for (j = 0; j < task; ++j) {
      a[i][j] = sin(0.00001 * a[i - 1][j + 1]);
    }
  }
  if (rank==0) {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < task; ++j) {
        a[i][j] = sin(0.00001 * b[i - 1][j + 1]);
      }
    }
  } else if (rank==size-1) {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < task-1; ++j) {
        a[i][j] = sin(0.00001 * b[i - 1][j + 1]);
      }
    }
  } else {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < task; ++j) {
        a[i][j] = sin(0.00001 * b[i - 1][j + 1]);
      }
    }
  }

  // Exchange - all to zero
  if (rank==0) {
    if (size>1) {
      MPI_Recv((void*)(net0[task+1]+1), N-1, MPI_DOUBLE, 1,
               0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send((void*)(net0[task]+1), N-1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    }
  } else if (rank==size-1) {
    if (rank%2==1) {
      MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
      MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
               0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
      MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
               0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
    }
  } else if (rank%2==1) {
    MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
    MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
             0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send((void*)(net0[task]+1), N-1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
    MPI_Recv((void*)(net0[task+1]+1), N-1, MPI_DOUBLE, rank+1,
             0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    MPI_Recv((void*)(net0[task+1]+1), N-1, MPI_DOUBLE, rank+1,
             0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send((void*)(net0[task]+1), N-1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
    MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
             0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
  }//exchange


  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0) {
    end=MPI_Wtime();
    printf("time=%lf sec\n", end-start);

    if (write) {
      ff = fopen("par-2.txt", "w");
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
    free(b[i]);
  }
  free(a);
  free(b);

  MPI_Finilize();

  return 0;
}
