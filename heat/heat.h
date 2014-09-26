#pragma once

#include <math.h>
#include <mpi.h>
#define TASK(n,m,k) (((n)-(k)) % (m) == 0)?\
                    (((n)-(k))/(m)):\
                    (((n)-(k))/(m) + 1)
int estimateNtau(int T,//total time
                 int N/*num of intervals on an axis*/) {
  return round(4*T*N*N)*2;
}

double uzero(double a, double b,
             double x, double y) {
  return exp(-(x*x - 2*b*x*y + y*y)/(a*a));
}

void printTemperatureNet(double** u, int N) {
  int i=0, j=0;
  for (j=0;j<N+1;j++) {
    for (i=0;i<N+1;i++) {
      printf("(%8.3f) ", u[i][j]);
    }
    printf("\n");
  }
}

double** allocNet(int N, int x, int y) {
    int i=0;
    double** array = (double**)malloc(sizeof(double*)*x);
    for(i=0;i<N+1;i++) {
      array[i] = (double*)malloc(sizeof(double)*y);
    }
    return array;
}

void initNet(int N, int arr_size, int from,
             double a, double b, double** array) {
  int i=0, j=0;
  for (i=0;i<arr_size;i++) {
    for (j=0;j<N+1;j++) {
      array[i][j] = uzero(a, b, ((double)(i+from))/N, ((double)j)/N);
    }
  }
}

double calcNode(double tau, int N,
                int i, int j,
                double** array) {
  return (tau*N*N)
         *(array[i-1][j]+array[i+1][j]
           +array[i][j-1]+array[i][j+1]
           -4*array[i][j]);
}

double** solveHeatEquation(int N, //num of intervals on an axis
                           double T, //total time
                           double a, double b,//coefficients for u(x,y,0)
                           int Ntau/*num of intervals on time axis*/) {
  int size = 0, rank = 0;

  MPI_Comm_size( MPI_COMM_WORLD, &size);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);
  
  //Determine elementary task
  int task = TASK(N-1, size, rank);
  
  //malloc
  double** net0, **net;
  int i = 0, j=0, arr_size=0;
  double tau = T/(double)Ntau;
  if (rank==0) {
      arr_size=N+1;
      net0 = allocNet(N, arr_size, N+1);    
      initNet(N, arr_size, 0, a, b, net0);
      net = allocNet(N, arr_size, N+1);    
  } else if (rank==size-1) {
      arr_size=task+2;
      net0 = allocNet(N, arr_size, N+1);    
      initNet(N, arr_size, N-task-1, a, b, net0);
      net = allocNet(N, arr_size, N+1);    
  } else {
      arr_size=task+2;
      net0 = allocNet(N, arr_size, N+1);    
      net = allocNet(N, arr_size, N+1);    
     
      int begin = 0;
      for (i=0;i<rank;i++)
        begin += TASK(N-1, size, i);
      initNet(N, arr_size, begin, a, b, net0);
  }

  

  //calculation
  int k=0, from=0;
 
  for (k=0;k<Ntau+1;k++) {
    for (i=1;i<task+1;i++) {
      for (j=1;j<N;j++) {
        net[i][j] = calcNode(tau, N, i, j, net0);
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (i=1;i<task+1;i++) {
      for (j=1;j<N;j++) {
        net0[i][j] = net[i][j];
      }
    }
    //exchange
    if (rank==0) {

    //exchange
      if (size>1) {
        MPI_Recv((void*)(net0[task+1]+1), N-1, MPI_DOUBLE, 1,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send((void*)(net0[task]+1), N-1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
      }
    } else if (rank == size-1) {
      if (rank%2 == 1) {
        MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
        MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      } else {
        MPI_Recv((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send((void*)(net0[0]+1), N-1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
      }
    } else if (rank%2 == 1) {
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
    } //exchange
  } // layer-loop
  fprintf(stderr, "-->[%d]calculation/exchange is over\n", rank);

  //gather
  if (rank>0) {
    for (i=0;i<task;i++)
      MPI_Send((void*)net0[i+1], N-1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  }
  
  if (rank==0) {
    int disp = 0;
    int taski = 0;
    for (i=1;i<size;i++) {
      taski = TASK(N-1, size, i);
      fprintf(stderr, "==>recv for proc[%d]\n", i);
      for (j=0;j<taski;j++){
        fprintf(stderr, "==>column %d from proc[%d]\n", j, i);
        fprintf(stderr, "task+1:%d, disp:%d, j:%d\n", task+1,disp,j);
        MPI_Recv((void*)net0[task+1+disp+j], N-1, MPI_DOUBLE, i,
                 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
      disp += TASK(N-1, size, i);
    }
  }
  fprintf(stderr, "-->[%d]gather is over\n", rank);

  MPI_Barrier(MPI_COMM_WORLD);
 
  if (rank<0) {
    for (i=0;i<task+2;i++) {
      fprintf(stderr, "-->[%d]column %d freed\n", rank, i);
      free(net0[i]);
      free(net[i]);
    }
    fprintf(stderr, "-->[%d]nets freed\n", rank);
    free(net0);
    free(net);
    fprintf(stderr, "-->[%d]free is over\n", rank);
  }
  
  
  if (rank==0) {
    for (i=0;i<N+1;i++) {
      fprintf(stderr, "==>column %d freed\n", i);
      free(net[i]);
    }
    fprintf(stderr, "==>net freed\n");
    free(net);
    fprintf(stderr, "==>free is over\n");
    return net0;
  }
  fprintf(stderr, "-->[%d]NULL is returned\n", rank);
  return NULL;
}
