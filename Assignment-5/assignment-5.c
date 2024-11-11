/*
Mutual Exclusion / Semaphores / Critical Section Techniques

Written by: Nathan Orgera (251175297)
Date: Mar 27th, 2023
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// Initialize global variable of 5 train stations with requested passengers (Station 0 -> 4)
int station[5] = {500, 0, 0, 0, 0}; // Goal: Reach same as desired

// Initialize global variable of capacities of desired passengers at train stations (Station 0 -> 4)
int desired[5] = {0, 50, 100, 250, 100};

// Initialize global variable for the trains
int train[2] = {0, 0}; // Maximums {100, 50}

// Initialize semaphore for STDOUT
sem_t stdoutSem;

// Create semaphores for stations 0-4
sem_t station0, station1, station2, station3, station4;

// Create method for train 0
void *train0Method(void *arg) {
    // For Train 0: Run loop until desired passengers are reached at each station
    while (station[0] != 0 || station[1] != 50 || station[2] != 100 || station[3] != 250 || station[4] != 100) {

        // If there are passengers at station 0, and the train has no passengers
        if (station[0] > 0 && train[0] == 0) {
            // Wait for station 0 semaphore
            sem_wait(&station0);
            // Wait for STDOUT semaphore
            sem_wait(&stdoutSem);

            // Print that train 0 has entered Station 0
            printf("Train 0 ENTERS Station 0\n");

            // Print how many passengers are at station 0
            printf("\tStation 0 has %d passengers to pick up\n", station[0]);

            // Update the value of train[0] by subtracting the number of passengers at station 0 to a max of 100
            if (station[0] < 100) {
                train[0] = train[0] + station[0];
            } else {
                train[0] = train[0] + 100;
            }

            // Print out picking up passengers and sleep
            printf("\tPicking up passengers...\n");
            float sleepTime = (float) train[0] / 20;
            unsigned int microSleepTime = (float) sleepTime * 1000000;
            usleep(microSleepTime);

            // Update the station value by subtracting by number of passengers picked up
            station[0] = station[0] - train[0];

            // Print out the status of the train
            printf("\tTrain 0 is at Station 0 and has %d/100 passengers\n", train[0]);

            // Print out remaining passengers to pick up
            printf("\tStation 0 has %d passengers to pick up\n", station[0]);

            // Print out leaving station 0
            printf("Train 0 LEAVES Station 0\n");

            // Release the station semaphore
            sem_post(&station0);
            // Release STDOUT semaphore
            sem_post(&stdoutSem);
            sleep(0.5);
        }
        // For stations 1-4 (the dropoff stations)
        else {
            // Try stations 1-4 in a loop until all passengers can be dropped off
            for (int i = 1; i < 5; i++) {
                // Empty train, no more dropoffs
                if (train[0] == 0) {
                    break;
                }
                // If the train has passengers and the desired capacity is not met at the station
                else if (desired[i] > station[i]) {
                    // Wait for station semaphore
                    if (i == 1) {
                        sem_wait(&station1);
                    } else if (i == 2) {
                        sem_wait(&station2);
                    } else if (i == 3) {
                        sem_wait(&station3);
                    } else if (i == 4) {
                        sem_wait(&station4);
                    }

                    // Wait for STDOUT semaphore
                    sem_wait(&stdoutSem);

                    // Print that train 0 has entered Station i
                    printf("Train 0 ENTERS Station %d\n", i);

                    // Find out how many passengers can be dropped off
                    int dropoff = 0;
                    // If train has less passengers than station can hold, set to train
                    if (train[0] < desired[i] - station[i]) {
                        dropoff = train[0];
                    } 
                    // If train has more passengers than station can hold, set to station
                    else {
                        dropoff = desired[i] - station[i];
                    }

                    // Print how many passengers Station i has to drop off
                    printf("\tStation %d has %d passengers to drop off\n", i, dropoff);

                    // Print out dropping off passengers and sleep
                    printf("\tUnloading passengers...\n");
                    float sleepTime = (float) dropoff / 20;
                    unsigned int microSleepTime = sleepTime * 1000000;
                    usleep(microSleepTime);

                    // Update the value of train[0] by subtracting by dropoff number
                    train[0] = train[0] - dropoff;

                    // Update the station value by adding the number of passengers dropped off
                    station[i] = station[i] + dropoff;

                    // Print out the status of the train
                    printf("\tTrain 0 is at Station %d and has %d/100 passengers\n", i, train[0]);

                    // Print out the status of the station
                    printf("\tStation %d has %d passengers to drop off\n", i, desired[i] - station[i]);

                    // Print out leaving station i
                    printf("Train 0 LEAVES Station %d\n", i);

                    // Release the semaphore
                    if (i == 1) {
                        sem_post(&station1);
                    } else if (i == 2) {
                        sem_post(&station2);
                    } else if (i == 3) {
                        sem_post(&station3);
                    } else if (i == 4) {
                        sem_post(&station4);
                    }
                    // Release STDOUT semaphore
                    sem_post(&stdoutSem);
                    sleep(0.5);
                }
            }
        }
    }
    return arg;
}

// Create method for train1
void *train1Method(void *arg) {
    // For Train 1: Run loop until desired passengers are reached at each station
    while (station[0] != 0 || station[1] != 50 || station[2] != 100 || station[3] != 250 || station[4] != 100) {

        // If there are passengers at station 0, and the train has no passengers
        if (station[0] > 0 && train[1] == 0) {
            // Wait for station 0 semaphore
            sem_wait(&station0);

            // Wait for STDOUT semaphore
            sem_wait(&stdoutSem);

            // Print that train 0 has entered Station 0
            printf("Train 1 ENTERS Station 0\n");

            // Print how many passengers are at station 0
            printf("\tStation 0 has %d passengers to pick up\n", station[0]);

            // Update the value of train[1] by subtracting the number of passengers at station 0 to a max of 50
            if (station[0] < 50) {
                train[1] = train[1] + station[0];
            } else {
                train[1] = train[1] + 50;
            }

            // Print out picking up passengers and sleep for 1 second
            printf("\tPicking up passengers...\n");
            float sleepTime = (float) train[1] / 20;
            unsigned int microSleepTime = sleepTime * 1000000;
            usleep(microSleepTime);

            // Update the station value by subtracting by number of passengers picked up
            station[0] = station[0] - train[1];

            // Print out the status of the train
            printf("\tTrain 1 is at Station 0 and has %d/50 passengers\n", train[1]);

            // Print out remaining passengers to pick up
            printf("\tStation 0 has %d passengers to pick up\n", station[0]);

            // Print out leaving station 0
            printf("Train 1 LEAVES Station 0\n");

            // Release the station semaphore
            sem_post(&station0);

            // Release STDOUT semaphore
            sem_post(&stdoutSem);
            sleep(0.5);
        }
        // For stations 1-4 (the dropoff stations)
        else {
            // Try stations 1-4 in a loop until all passengers can be dropped off
            for (int i = 1; i < 5; i++) {
                // Empty train, no more dropoffs
                if (train[1] == 0) {
                    break;
                }
                // If the train has passengers and the desired capacity is not met at the station
                else if (desired[i] != station[i]) {
                    // Wait for station semaphore
                    if (i == 1) {
                        sem_wait(&station1);
                    } else if (i == 2) {
                        sem_wait(&station2);
                    } else if (i == 3) {
                        sem_wait(&station3);
                    } else if (i == 4) {
                        sem_wait(&station4);
                    }

                    // Wait for STDOUT semaphore
                    sem_wait(&stdoutSem);

                    // Print that train 0 has entered Station i
                    printf("Train 1 ENTERS Station %d\n", i);

                    // Find out how many passengers can be dropped off (train capacity or station capacity minimum)
                    int dropoff = 0;
                    if (train[1] < desired[i] - station[i]) {
                        dropoff = train[1];
                    } else {
                        dropoff = desired[i] - station[i];
                    }

                    // Print how many passengers Station i has to drop off
                    printf("\tStation %d has %d passengers to drop off\n", i, dropoff);

                    // Print out dropping off passengers and sleep for 1 second
                    printf("\tUnloading passengers...\n");
                    float sleepTime = (float) dropoff / 20;
                    unsigned int microSleepTime = sleepTime * 1000000;
                    usleep(microSleepTime);

                    // Update the value of train[0] by subtracting by dropoff number
                    train[1] = train[1] - dropoff;

                    // Update the station value by adding the number of passengers dropped off
                    station[i] = station[i] + dropoff;

                    // Print out the status of the train
                    printf("\tTrain 1 is at Station %d and has %d/50 passengers\n", i, train[1]);

                    // Print out the status of the station
                    printf("\tStation %d has %d passengers to drop off\n", i, desired[i] - station[i]);

                    // Print out leaving station i
                    printf("Train 1 LEAVES Station %d\n", i);

                    // Release the semaphore
                    if (i == 1) {
                        sem_post(&station1);
                    } else if (i == 2) {
                        sem_post(&station2);
                    } else if (i == 3) {
                        sem_post(&station3);
                    } else if (i == 4) {
                        sem_post(&station4);
                    }
                    // Release STDOUT semaphore
                    sem_post(&stdoutSem);
                    sleep(0.5);
                }
            }
        }
    }
    return arg;
}


int main(void) {
    // Initialize semaphores for each train station and STDOUT
    sem_init(&station0, 0, 1);
    sem_init(&station1, 0, 1);
    sem_init(&station2, 0, 1);
    sem_init(&station3, 0, 1);
    sem_init(&station4, 0, 1);
    sem_init(&stdoutSem, 0, 1);

    // Create two threads, one for each train
    pthread_t train0, train1;

    // Make sure thread creation works and call threads
    if (pthread_create(&train0, NULL, train0Method, NULL) != 0) {
        printf("Error creating thread 0\n");
    }
    sleep(1);
    if (pthread_create(&train1, NULL, train1Method, NULL) != 0) {
        printf("Error creating thread 1\n");
    }

    // Wait for the threads to finish
    pthread_join(train0, NULL);
    pthread_join(train1, NULL);

    // Destroy semaphores
    sem_destroy(&station0);
    sem_destroy(&station1);
    sem_destroy(&station2);
    sem_destroy(&station3);
    sem_destroy(&station4);
    sem_destroy(&stdoutSem);

    // Return
    return 0;
}
