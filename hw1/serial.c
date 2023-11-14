#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <M>\n", argv[0]);
        return 1;
    }

    int M = atol(argv[1]);
    int N = M / 2;
int prime[N];
  int j;
  int k;
  int n;
  int quo, rem;
  
  clock_t tic = clock(); 

P1:
  prime[0] = 2;
  n = 3;
  j = 0;
P2:
  j = j + 1;
  prime[j] = n;
P3:
  if (j == (N - 1))
    goto P9;
P4:
  n = n + 2;
P5:
  k = 1;
// is prime 
P6:
  quo = n / prime[k];
  rem = n % prime[k];
  if (rem == 0)
    goto P4;
P7:
  if (quo <= prime[k])
    goto P2;
P8:
  k = k + 1;
  goto P6;
P9:
  for (j = 0; j < N && prime[j] < M; j++)
    printf("%d\n", prime[j]);

  clock_t toc = clock();

  printf("%f\n", (double)(toc - tic) / CLOCKS_PER_SEC);
  return 0;
}
