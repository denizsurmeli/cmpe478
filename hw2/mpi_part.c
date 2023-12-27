#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int is_prime(int n, int *primes, int k) {
  if (n <= 1) {
    return 0;
  }

  if (n == 2) {
    return 1;
  }

  int quo, rem;
  for (int j = 0; j < k; j++) {
    quo = n / primes[j];
    rem = n % primes[j];

    if (rem == 0) {
      return 0;
    }

    if (quo <= primes[j]) {
      return 1;
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <M>\n", argv[0]);
    return 1;
  }

  int M = atoi(argv[1]);
  int N;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &N);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double start = MPI_Wtime();

  int *primes = (int *)calloc(sqrt(M) + 1, sizeof(int));
  int s = sqrt((double)M) + 1;
  int n = 3;
  int k = 1;

  if (rank == 0) {
    primes[0] = 2;
    while (n <= s) {
      int p = is_prime(n, primes, k);
      if (p) {
        primes[k++] = n;
      }
      n += 2;
    }

    for (int i = 1; i < N; i++) {
      MPI_Send(&k, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
      MPI_Send(primes, s, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
  } else {
    MPI_Recv(&k, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(primes, s, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Barrier(MPI_COMM_WORLD);
  }

  int chunk_size = M / N;
  int start_num = rank * chunk_size;
  int end_num = (rank == N - 1) ? M : start_num + chunk_size - 1;

  int *local_primes = (int *)calloc(
      end_num - start_num, sizeof(int)); // Max possible primes in a chunk
  int local_count = 0;

  for (int i = start_num; i <= end_num; i++) {
    if (i % 2 == 0 && i != 2) {
      continue;
    }

    if (is_prime(i, primes, k)) {
      local_primes[local_count++] = i;
    }
  }

  int all_count = 0;
  MPI_Reduce(&local_count, &all_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  int *positions = (int *)calloc(N, sizeof(int));
  int *displs = (int *)calloc(N, sizeof(int));

  MPI_Gather(&local_count, 1, MPI_INT, positions, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  int *all_primes = NULL;
  if (rank == 0) {
    displs[0] = 0;
    for (int i = 1; i < N; i++) {
      displs[i] = displs[i - 1] + positions[i - 1];
    }
    all_primes = (int *)calloc(all_count, sizeof(int));
  }

  MPI_Gatherv(local_primes, local_count, MPI_INT, all_primes, positions, displs,
              MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    for (int i = 0; i < all_count; i++) {
      printf("%d\n", all_primes[i]);
    }

    free(all_primes);
      double end = MPI_Wtime();
      printf("%f\n", end - start);

  }

  free(local_primes);
  free(positions);
  free(displs);
  MPI_Finalize();
  return 0;
}
