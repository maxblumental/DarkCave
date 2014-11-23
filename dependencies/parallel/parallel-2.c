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
  int my_task = task(N-1, size, rank),
      offset = 0;
  for (i = 0; i < rank; ++i)
    offset += task(N-1, size, i);

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

  // Parallelize - uses it's own limits
  if (rank==0) {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < my_task; ++j) {
        if (i > 1 && j == my_task - 1 && size > 1)
          MPI_Recv((void*)(a[i-1]+my_task), 1, MPI_DOUBLE, 1,
                   0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        a[i][j] = sin(0.00001 * a[i - 1][j + 1]);
      }
    }
  } else if (rank==size-1) {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < my_task; ++j) {
       a[i][j] = sin(0.00001 * a[i - 1][j + 1]);
       if (i < N-1 && j == 0 )
          MPI_Send((void*)a[i], 1, MPI_DOUBLE,
                   rank-1, 0, MPI_COMM_WORLD);
      }
    }
/*  } else if (rank % 2 == 1) {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < my_task; ++j) {
        if (i < N-1 && j == 1) {
          MPI_Send((void*)a[i], 1, MPI_DOUBLE,
                   rank-1, 0, MPI_COMM_WORLD);
        } else if (i < N-1 && my_task == 1) {
          MPI_Send((void*)a[i-1], 1, MPI_DOUBLE,
                   rank-1, 0, MPI_COMM_WORLD);
        }
        a[i][j] = sin(0.00001 * a[i - 1][j + 1]);
        if (i > 1 && my_task > 1 && j == my_task - 2) {
          MPI_Recv((void*)(a[i-1]+my_task), 1, MPI_DOUBLE, rank+1,
                   0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if (i > 1 && my_task == 1) {
          MPI_Recv((void*)(a[i]+my_task), 1, MPI_DOUBLE, rank+1,
                   0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
      }
    }*/
  } else {
    for (i = 1; i < N; ++i) {
      for (j = 0; j < my_task; ++j) {
        if (i > 1 && j == my_task - 1)
          MPI_Recv((void*)(a[i-1]+my_task), 1, MPI_DOUBLE, rank+1,
                   0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        a[i][j] = sin(0.00001 * a[i - 1][j + 1]);
        if (i < N-1 && j == 0)
          MPI_Send((void*)a[i], 1, MPI_DOUBLE,
                   rank-1, 0, MPI_COMM_WORLD);
      }
    }
  }


  if (rank>0) {
    for (i=0;i<N;i++)
      MPI_Send((void*)a[i], my_task, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  
  if (rank==0) {
    int disp=0;
    int taski=0;
    for (i=1;i<size;i++) {
      taski=task(N-1, size, i);
      for (j=0;j<N;j++){
        MPI_Recv((void*)(a[j]+disp+my_task), taski, MPI_DOUBLE, i,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      disp+=task(N-1, size, i);
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0) {
    end=MPI_Wtime();
    printf("%lf\n", end-start);

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
  }
  free(a);

  MPI_Finalize();

  return 0;
}
