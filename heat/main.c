#include <stdio.h>
#include <stdlib.h>
#include "heat.h"

int main(int argc, char** argv) {
  
  if (argc != 5) {
    fprintf(stderr, "$ mpirun -n <n_of_processes> ./a.out T N a b\n");
    return 0;
  }

  MPI_Init(NULL,NULL);
  
  int size=0, rank=0;

  MPI_Comm_size( MPI_COMM_WORLD, &size);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);

  double T=atof(argv[1]),
         a=atof(argv[3]),
         b=atof(argv[4]);
  int N=atoi(argv[2]);
 
  int Ntau=estimateNtau(T, N);

  double start=0, end=0;
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0)
    start=MPI_Wtime();

  /*Build the system and solve it*/
  double** solution=solveHeatEquation(N, T, a, b, Ntau);
  
  if (rank==0) {
    printTemperatureNet(solution, N+1, N+1);
    int i=0;
    for (i=0;i<N+1;i++) {
      free(solution[i]);
    }
    free(solution);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0) {
    end=MPI_Wtime();
    printf("time=%lf sec\n", end-start);
  }

  MPI_Finalize();

  return 0;
}
