#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
int main(int argc, char** argv) {
  
  if (argc!=4) {
    printf("Usage: $./a.out a b n_of_threads\n");
    return 0;
  }

  double a = atof(argv[1]),
         b = atof(argv[2]);
  int tn = atoi(argv[3]);

  omp_set_num_threads(tn);

  double integral = 0;
  int i=0, N=100000000;

  clock_t t1 = clock();

  #pragma omp parallel for reduction(+:integral) private(i)
  for (i=0;i<N;i++) {
    integral += sin(1/(a+(b-a)*i/N))*((b-a)/N);
  }

  clock_t t2 = clock();

  printf("I=%f, dt=%f\n", integral, (float)(t2 - t1) / CLOCKS_PER_SEC);

  return 0;  
}
