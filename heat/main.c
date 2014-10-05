#include <stdio.h>
#include <stdlib.h>
#include "heat.h"

int main(int argc, char** argv) {
  
  if (argc != 5) {
    fprintf(stderr, "$ mpirun -n <n_of_processes> ./a.out T N a b\n");
    return 0;
  }

  MPI_Init(NULL,NULL);
  
  int size = 0, rank = 0;

  MPI_Comm_size( MPI_COMM_WORLD, &size);
  MPI_Comm_rank( MPI_COMM_WORLD, &rank);

  /*Get the args:
   *OUT: T N a b*/
  double T = atof(argv[1]),
         a = atof(argv[3]),
         b = atof(argv[4]);
  int N = atoi(argv[2]);
  /*Estimate tau:
   *OUT: tau*/
  int Ntau = estimateNtau(T, N);
  fprintf(stderr, "{main}-->[%d]BEGIN\n", rank);
  /*Get num of processes:
   *OUT: n*/

  /*Build the system and solve it:
   *IMPL: loop T/tau, (N+1)(N+1) arrays
   *INPUT: T N a b tau
   *OUT: double** u[N+1][N+1] for t = T*/
  double** solution = solveHeatEquation(N, T, a, b, Ntau);
  fprintf(stderr, "{main}-->[%d]solved\n", rank);
  
  /*Output:
   *IMPL: array printer.*/
 
  if (rank == 0) {
    fprintf(stderr, "{main}==>right before print\n");
    printTemperatureNet(solution, N+1, N+1);
    int i=0;
    for (i=0;i<N+1;i++) {
//      fprintf(stderr, "{main}==>column %d freed\n", i);
      free(solution[i]);
    }
    fprintf(stderr, "{main}==>net freed\n");
    free(solution);
  }

  MPI_Finalize();

  return 0;
}
