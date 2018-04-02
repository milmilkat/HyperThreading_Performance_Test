#include <iostream>
#include <time.h>
#include <stdint.h>
#include <cstdlib>
#include <pthread.h>
#include <new>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

using namespace std;

int NumberOfThreads;
int ThreadLoad;

struct prime_range {
    int min;
    int max;
    int total;
};

void* findPrime(void *threadarg)
{
    int i, primes = 0;
    struct prime_range *this_range;
    this_range = (struct prime_range *) threadarg;

    int minLimit =  this_range -> min ;
    int maxLimit =  this_range -> max ;
    int flag = false;
    while (minLimit <= maxLimit) {
        i = 2;
        int lim = ceil(sqrt(minLimit));
        while (i <= lim) {
            if (minLimit % i == 0){
                flag = true;
                break;
            }
            i++;
        }
        if (!flag){
            primes++;
        }
        flag = false;
        minLimit++;
    }
    this_range ->total = primes;
    pthread_exit(NULL);
}

int main()
{
    struct timespec start, finish;
    double elapsed;

    cout << "Please enter the number of Threads: ";
    cin >> NumberOfThreads;
    cout << "Please enter the load of each thread: ";
    cin >> ThreadLoad;

    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[NumberOfThreads];
    struct prime_range pr[NumberOfThreads];
    int rc;
    pthread_attr_t attr;
    void *status;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(int t=1; t<= NumberOfThreads; t++){
        pr[t].min = (t-1) * ThreadLoad + 1;
        pr[t].max = t*ThreadLoad;
        rc = pthread_create(&threads[t], NULL, findPrime,(void *)&pr[t]);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    int totalPrimesFound = 0;
    // free attribute and wait for the other threads
    pthread_attr_destroy(&attr);
    for(int t=1; t<= NumberOfThreads; t++){
        rc = pthread_join(threads[t], &status);
        if (rc) {
            printf("Error:unable to join, %d" ,rc);
            exit(-1);
        }
        totalPrimesFound += pr[t].total;
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("This machine calculated all %d prime numbers under %d in %lf seconds\n",
           totalPrimesFound, NumberOfThreads*ThreadLoad, elapsed);
    pthread_exit(NULL);

    return 0;
}
