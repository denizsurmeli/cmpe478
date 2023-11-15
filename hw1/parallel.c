#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

// algorithm:
// 1) Generate primes up to sqrt(M).
// 2) Using each thread, split the remaining
// interval sqrt(M)-M into N chunks.
// 3) Each thread searches for the primes in their
// interval, collects into a local array
// 4) Collect the results into one array, you are done

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

int main(int argc,char **argv){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <M>\n", argv[0]);
        return 1;
    }

    int M = atol(argv[1]);
    // there can be atmost M/2 prime numbers up to M
    int N = M / 2;
    
    int* primes = (int*) calloc(N, sizeof(int));
    primes[0] = 2;

    int s = sqrt((double) M) + 1; 
    int n = 3;
    int k = 1;
    int quo, rem;

    double start = omp_get_wtime();
    int nthreads;  
    while(k < N && n <= s) {
        int p = is_prime(n, primes, k);
        if (p) {
            primes[k++] = n; 
        }        
        n += 2;                 
    }

    #pragma omp parallel for schedule(runtime)
    for(int n = primes[k-1] + 2;n <= M; n +=2) {
       int p = is_prime(n, primes, k);
       if(p) {
            #pragma omp critical
                primes[k++] = n;
       }
    }

    for(int i = 0 ; i < k; i++) {
        printf("%d\n", primes[i]);
    }

    free(primes);

    double elapsed = omp_get_wtime() - start;
    printf("%f\n", elapsed);
    return 0;
}
