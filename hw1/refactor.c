#include <stdio.h>

#define N 50000 
#define M 100000
long prime[N] = {0};

int is_prime(long n, long *primes, long k) {
    int quo, rem;
     for (long j = 0 ; j < k; j++){
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

int main() {
    prime[0] = 2;
    long n = 3;
    long k = 1;
    long quo, rem;

    while(k < N && n <= M) {
        int p = is_prime(n, prime, k);
        if (p) {
            prime[k++] = n; 
        }
        n+=2;
    }

    for(long i = 0 ; i < k; i++) {
        printf("%ld\n", prime[i]);
    }
}
