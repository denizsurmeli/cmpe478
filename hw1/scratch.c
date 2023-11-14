#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#define M 100000
int main() {
    long N = M / 2;

    long* primes = (long*)calloc(N, sizeof(long));
    primes[0] = 2;

    long s = sqrt((double)N);
    long n = 3;
    long k = 1;
    long quo, rem;

    int nthreads;
    while (k < N && n <= s) {
        for (long j = 1; j < k + 1; j++) {
            quo = n / primes[j];
            rem = n % primes[j];

            if (rem == 0) {
                break;
            }

            if (quo <= primes[j]) {
                primes[k] = n;
                k++;
                break;
            }
        }

        n += 2;
    }

    for (long i = 0; i < k; i++) {
        printf("%ld\n", primes[i]);
    }

    long start = primes[k - 1] + 1;
    long end = M;

    #pragma omp parallel private(quo, rem, n)
    {
        int tid = omp_get_thread_num();
        #pragma omp single
        nthreads = omp_get_num_threads();

        long chunksize = (end - start + 1) / nthreads; // +1 to include the endpoint
        long lstart = start + tid * chunksize;
        if (lstart % 2 == 0) {
            lstart++;
        }
        long lend = (tid == nthreads - 1) ? end : lstart + chunksize - 1;
        if (lend % 2 == 0) {
            lend--;
        }

        long f = 0;
        long* lprimes = (long*)calloc(lend - lstart , sizeof(long));

        while (lstart <= lend) {
            for (long j = 0; j < k; j++) {
                quo = lstart / primes[j];
                rem = lstart % primes[j];

                if (rem == 0) {
                    break;
                }

                if (quo <= primes[j]) {
                    lprimes[f] = lstart;
                    f++;
                    break;
                }
            }
            lstart += 2;
        }

        for (int y = 0; y < f; y++) {
            printf("%ld\n", lprimes[y]);
        }

        free(lprimes);
    }

    free(primes);

    return 0;
}
