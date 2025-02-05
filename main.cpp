#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

#define THREADS 4

typedef long long ll;

ll factorial_serial(int n) {
    if(n < 0) {
        throw std::invalid_argument("n < 0");
    }
    else if(n <= 1) {
        return 1;
    }
    else {
        ll r = 1;
        for(int i = 2; i <= n; i++) {
            r *= i;
        }
        return r;
    }
}

std::mutex mtx;

void subfactorial(std::vector<ll>& result, ll start, int length, int limit) {
    if(start > limit) {
        return;
    }
    ll r = start;
    for(int i = 1; i < length; i++) {
        if(start + i > limit) {
            break;
        }
        r *= (start + i);
    }
    std::lock_guard<std::mutex> lock(mtx);
    result.push_back(r);
}

// 1 2 3 4 5 6 7 8
ll factorial_parallel(int n, int tn) {
    if(n < 0) {
        throw std::invalid_argument("n < 0");
    }
    else if(n <= 1) {
        return 1;
    }
    else {
        ll r = 1;

        int l = std::ceil(1.0 * n / tn);

        std::vector<std::thread> threads;
        std::vector<ll> results;

        for(int i = 0; i < tn; i++) {
            threads.emplace_back(std::thread(subfactorial, std::ref(results), l * i + 1, l, n));
        }

        for(auto& t : threads) {
            t.join();
        }

        for(ll rr : results) {
            r *= rr;
        }

        return r;
    }
}

int main(int argc, const char** argv) {
    int tn = THREADS;
    if(argc > 1) {
        tn = std::atoi(argv[1]);
    }

    int n;
    std::cout << "Factorial of: ";
    std::cin >> n;

    ll serial   = factorial_serial(n);
    ll parallel = factorial_parallel(n, tn);

    assert(serial == parallel);

    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "SERIAL: " << factorial_serial(n) << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Took: " << dur.count() << "mcs" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    std::cout << "PARALLEL: " << factorial_parallel(n, tn) << std::endl;
    stop = std::chrono::high_resolution_clock::now();
    dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Took: " << dur.count() << "mcs" << std::endl;
    return 0;
}
