#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/remove.h>
#include <thrust/sequence.h>
#include <cmath>
#include <iostream>
#include <chrono>

inline bool is_prime(int n, const thrust::host_vector<int> &primes) {
    if (n <= 1) {
        return false;
    }
    
    if (n == 2) {
        return true;
    }

    int quo, rem;
     for (int p: primes){ 
         quo = n / p;
         rem = n % p;

         if (rem == 0) {
            return false;
         }

         if (quo <= p) {
            return true;
         }
     }
     return false;
}


struct is_prime_functor {
    const thrust::host_vector<int> primes;

    is_prime_functor(const thrust::host_vector<int> &_p) : primes(_p) {}
    __host__ __device__
        bool operator() (const int k) {
            return !is_prime(k, primes);
        }
};

int main(int argc, char* argv[]) 
{

    // bootstrap bootstrap_primes
    if (argc < 2) {
        std::cout << "Please provide a number as argument" << std::endl;
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();
    int M = std::atol(argv[1]);
    int N = M / 2;

    thrust::host_vector<int> bootstrap_primes(N, 0); // Initialize a host vector with N elements
    thrust::device_vector<int> primes(M);
    thrust::sequence(primes.begin(), primes.end());

    
    bootstrap_primes[0] = 2;

    int s = static_cast<int>(std::sqrt(static_cast<double>(M))) + 1;
    int n = 3;
    int k = 1;

    while (k < N && n <= s) {
        bool p = is_prime(n, bootstrap_primes);
        if (p) {
            bootstrap_primes[k++] = n;
        }
        n += 2;
    }

    auto end = thrust::remove_if(primes.begin(), primes.end(), is_prime_functor(bootstrap_primes));
    auto elapsed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
    for(auto v = primes.begin(); v != end; v++){
        std::cout<<*v<<std::endl;
    }
    std::cout<<std::fixed<<elapsed.count()<< std::endl;
    return 0;
}
