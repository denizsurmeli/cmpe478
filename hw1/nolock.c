#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// #define M 100000
int is_prime(int n, int *primes, int k) {
    int quo, rem;
     for (int j = 0 ; j < k; j++){
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

    int M = atol(argv[1]);
    int N = M / 2;
    int* primes = (int*) calloc(N, sizeof(int));
    primes[0] = 2;
    int s = sqrt((double) M) + 1; 
    int n = 3;
    int k = 1;

    double start = omp_get_wtime();
    // Find primes up to sqrt(M)
    while(k < N && n <= s) {
        if (is_prime(n, primes, k)) {
            primes[k++] = n;
        }        
        n += 2;
    }

    #pragma omp parallel
    {
        int* local_primes = (int*) calloc(N, sizeof(int));
        int local_k = 0;

        #pragma omp for schedule(runtime)
        for(int n = primes[k-1] + 2; n <= M; n += 2) {
            if(is_prime(n, primes, k)) {
                local_primes[local_k++] = n;
            }
        }
     
        for(int i = 0 ; i < local_k; ++i){
            printf("%d\n", local_primes[i]);
        }

        free(local_primes);
    }

    // Print all primes found
    for(int i = 0; i < k; i++) {
        printf("%d\n", primes[i]);
    }

    free(primes);

    double elapsed = omp_get_wtime() - start;
    printf("%f\n", elapsed);
    return 0;
}

