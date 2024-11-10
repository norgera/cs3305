/*
Multi-threaded Prime Number Calculator
- Finds prime numbers up to a given limit
- Uses multiple threads to count and sum the primes
- Handles numbers up to 999,999

Author: Nathan Orgera
Date: Mar 7th, 2023
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long long globalCount = 0; // Global count of primes
long long globalSum = 0;   // Global sum of primes

// Struct to hold thread data
typedef struct {
    int start;
    int end;
    int curThread;
} thread_data;

// Function to check if a number is prime
int is_prime(int num) {
    if (num <= 1) return 0; // 0 and 1 are not prime
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return 0; // Not prime
    }
    return 1; // Is prime
}

// Function to calculate number of primes from start to end in range of numbers
void * countPrimes(void * data) {

    // Get start, end, and thread number from input struct
    thread_data *info = (thread_data *)data;
    int start = info->start;
    int end = info->end;
    int curThread = info->curThread;

    // Initialize count and sum variables for thread
    long long count = 0;
    long long sum = 0;

    // Print out Thread information
    printf("Thread # %d is finding primes from low = %d to high = %d\n", curThread, start, end);

    // Get the primes up to N for the interval [start, end]
    for (int i = start; i <= end; i++) {
        if (is_prime(i)) {
            count++;
            sum += i;
        }
    }

    // Store the number of primes and their sum in the global variables
    globalCount += count;
    globalSum += sum;

    // Print out thread information
    printf("Sum of thread %d is %lld, Count is %lld\n", curThread, sum, count);
    free(info);
    return NULL;
}

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Proper usage is ./assignment-3 <threadCount> <highestInt>\n");
        return 1;
    }

    // Accept number of threads and max number to calculate primes up to (denoted N) from command line
    int numThreads = atoi(argv[1]);
    int N = atoi(argv[2]);

    // Create array of threads and initialize start / end variables for range
    pthread_t threads[numThreads];
    int start = 0;
    int end = (N + numThreads - 1) / numThreads;

    // Create the threads and call countPrimes on their range
    for (int i = 0; i < numThreads; i++) {

        // Add thread data to thread_data struct through a pointer
        thread_data *info = malloc(sizeof(thread_data));
        info->start = start;
        info->end = end < N ? end : N; // Ensure we don't exceed N
        info->curThread = i;

        // Create the thread and call countPrimes, checking if successful
        if (pthread_create(&threads[i], NULL, countPrimes, info)) {
            printf("Error with creating thread\n");
            return 1;
        }

        // Update the start and end values for the next thread
        start = end + 1; // Move to the next number
        end = start + ((N + numThreads - 1) / numThreads) - 1; // Calculate new end
    }

    // Loop through threads and join them
    for (int i = 0; i < numThreads; i++) { 
        
        // Wait for the threads to finish
        if (pthread_join(threads[i], NULL)) {
            printf("Error with joining thread\n");
            return 1;
        }
    }

    // Print final sum and count
    printf("\n        GRAND SUM IS %lld, COUNT IS %lld\n", globalSum, globalCount);
    return 0;
}
