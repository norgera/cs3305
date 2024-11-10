/*
CPU scheduling algorithm simulator to calculate average wait and turnaround times
using First Come First Served, Shortest Job First, and Round Robin algorithms.

Written by: Nathan Orgera (251175297)
Date: Mar 21st, 2023
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#define MAX_PROCESSES 20 
#define MAX_LINE_LENGTH 1024 

// Structure to hold process information and metrics
typedef struct {
    char name[3];              // Process identifier (e.g., "P0", "P1")
    int arrival_time;          // Time when process arrives (all 0 for this implementation)
    int burst_time;            // Total CPU time needed
    int wait_time;             // Time spent waiting before completion
    int turnaround_time;       // Total time from arrival to completion
    bool finished;             // Whether process has completed execution
} process_t;

int main(int argc, char *argv[]) {
    
    // Validate arguments: program_name + algorithm + input_file
    if (argc < 3) {
        printf("Insufficient arguments!\n");
        return 1;
    }
    
    char *input_filename = argv[argc - 1];
    
    // First Come First Served
    if (strcmp(argv[1], "-f") == 0) {
        printf("First Come First Served\n");
        process_t process_list[MAX_PROCESSES];
        int process_count = 0;
        FILE *input_file = fopen(input_filename, "r");

        if (!input_file) {
            fprintf(stderr, "Failed to open file\n");
            return 1;
        }

        // Read process data from input file
        char input_line[MAX_LINE_LENGTH];
        while (fgets(input_line, MAX_LINE_LENGTH, input_file)) {
            char *process_name = strtok(input_line, ",");
            char *burst_time_str = strtok(NULL, ",");
            
            // Initialize process attributes
            strncpy(process_list[process_count].name, process_name, 3);
            process_list[process_count].burst_time = atoi(burst_time_str);
            process_list[process_count].arrival_time = 0;
            process_list[process_count].wait_time = 0;
            process_list[process_count].turnaround_time = 0;
            process_count++;
        }
        fclose(input_file);

        // Execute processes in order of arrival
        int current_time = 0;
        for (int i = 0; i < process_count; i++) {
            process_t *current_process = &process_list[i];
            current_process->wait_time = current_time;

            // Show process status for each time unit of execution
            for (int time_unit = 0; time_unit < current_process->burst_time; time_unit++) {
                printf("T%d : %s - Burst left %2d, Wait time %2d, Turnaround time %2d\n",
                    current_time + time_unit,
                    current_process->name,
                    current_process->burst_time - time_unit,
                    current_process->wait_time,
                    current_time + current_process->burst_time);
            }
            
            current_time += current_process->burst_time;
            current_process->turnaround_time = current_time;
        }

        // Calculate and display final statistics
        printf("\n");
        double total_wait_time = 0;
        double total_turnaround_time = 0;
        
        for (int i = 0; i < process_count; i++) {
            total_wait_time += process_list[i].wait_time;
            total_turnaround_time += process_list[i].turnaround_time;
            
            // Display individual process statistics
            printf("%s\n", process_list[i].name);
            printf("\tWaiting time:           %d\n", process_list[i].wait_time);
            printf("\tTurnaround time:        %d\n", process_list[i].turnaround_time);
            printf("\n");
        }
        
        // Display average metrics
        printf("Total average waiting time:     %.1f\n", total_wait_time / process_count);
        printf("Total average turnaround time:  %.1f\n", total_turnaround_time / process_count);
        
        return 0;
    }

    // Shortest Job First Algorithm
    else if (strcmp(argv[1], "-s") == 0) {
        printf("Shortest Job First\n");
        process_t process_list[MAX_PROCESSES];
        int process_count = 0;
        FILE *input_file = fopen(input_filename, "r");
        
        if (!input_file) {
            fprintf(stderr, "Failed to open file\n");
            return 1;
        }

        // Read and initialize processes from input file
        char input_line[MAX_LINE_LENGTH];
        while (fgets(input_line, MAX_LINE_LENGTH, input_file)) {
            char *process_name = strtok(input_line, ",");
            char *burst_time_str = strtok(NULL, ",");
            
            strncpy(process_list[process_count].name, process_name, 3);
            process_list[process_count].burst_time = atoi(burst_time_str);
            process_list[process_count].arrival_time = 0;
            process_list[process_count].wait_time = 0;
            process_list[process_count].turnaround_time = 0;
            process_list[process_count].finished = false;
            process_count++;
        }
        fclose(input_file);

        // Sort processes by burst time (shortest first)
        for (int i = 0; i < process_count - 1; i++) {
            for (int j = 0; j < process_count - i - 1; j++) {
                if (process_list[j].burst_time > process_list[j + 1].burst_time) {
                    process_t temp = process_list[j];
                    process_list[j] = process_list[j + 1];
                    process_list[j + 1] = temp;
                }
            }
        }

        // Execute processes in order of shortest burst time
        int current_time = 0;
        for (int i = 0; i < process_count; i++) {
            process_list[i].wait_time = current_time;
            
            // Show process status for each time unit
            for (int time_unit = 0; time_unit < process_list[i].burst_time; time_unit++) {
                printf("T%d : %s - Burst left %2d, Wait time %2d, Turnaround time %2d\n",
                    current_time + time_unit,
                    process_list[i].name,
                    process_list[i].burst_time - time_unit,
                    process_list[i].wait_time,
                    current_time + process_list[i].burst_time);
            }
            
            current_time += process_list[i].burst_time;
            process_list[i].turnaround_time = current_time;
        }

        // Calculate and display final statistics
        printf("\n");
        double total_wait_time = 0;
        double total_turnaround_time = 0;
        
        for (int i = 0; i < process_count; i++) {
            total_wait_time += process_list[i].wait_time;
            total_turnaround_time += process_list[i].turnaround_time;
            
            printf("%s\n", process_list[i].name);
            printf("\tWaiting time:           %d\n", process_list[i].wait_time);
            printf("\tTurnaround time:        %d\n", process_list[i].turnaround_time);
            printf("\n");
        }
        
        printf("Total average waiting time:     %.1f\n", total_wait_time / process_count);
        printf("Total average turnaround time:  %.1f\n", total_turnaround_time / process_count);

        return 0;
    }

    // Round Robin
    else if (strcmp(argv[1], "-r") == 0 && argc == 4) {
        int time_quantum = atoi(argv[2]);
        if (time_quantum <= 0) {
            printf("Invalid time quantum!\n");
            return 1;
        }

        printf("Round Robin with quantum %d\n", time_quantum);
        process_t process_list[MAX_PROCESSES];
        int process_count = 0;
        FILE *input_file = fopen(input_filename, "r");
        
        if (!input_file) {
            fprintf(stderr, "Failed to open file\n");
            return 1;
        }

        // Read and initialize processes
        char input_line[MAX_LINE_LENGTH];
        while (fgets(input_line, MAX_LINE_LENGTH, input_file)) {
            char *process_name = strtok(input_line, ",");
            char *burst_time_str = strtok(NULL, ",");
            
            strncpy(process_list[process_count].name, process_name, 3);
            process_list[process_count].burst_time = atoi(burst_time_str);
            process_list[process_count].arrival_time = 0;
            process_list[process_count].wait_time = 0;
            process_list[process_count].turnaround_time = 0;
            process_list[process_count].finished = false;
            process_count++;
        }
        fclose(input_file);

        // Execute processes using Round Robin scheduling
        int current_time = 0;
        int unfinished_processes = process_count;
        int *remaining_burst_time = malloc(process_count * sizeof(int));
        
        // Initialize remaining burst time for each process
        for (int i = 0; i < process_count; i++) {
            remaining_burst_time[i] = process_list[i].burst_time;
        }

        // Continue until all processes complete
        while (unfinished_processes > 0) {
            for (int i = 0; i < process_count; i++) {
                if (remaining_burst_time[i] > 0) {
                    // Determine execution time for this quantum
                    int execution_time = (remaining_burst_time[i] > time_quantum) ? 
                        time_quantum : remaining_burst_time[i];
                    
                    remaining_burst_time[i] -= execution_time;
                    current_time += execution_time;

                    printf("T%d : %s - Burst left %2d, ", 
                        current_time, 
                        process_list[i].name, 
                        remaining_burst_time[i]);

                    // Process has completed
                    if (remaining_burst_time[i] == 0) {
                        process_list[i].turnaround_time = current_time;
                        process_list[i].wait_time = process_list[i].turnaround_time - 
                            process_list[i].burst_time;
                        unfinished_processes--;
                        
                        printf("Wait time %2d, Turnaround time %2d\n",
                            process_list[i].wait_time,
                            process_list[i].turnaround_time);
                    } else {
                        printf("Still executing...\n");
                    }
                }
            }
        }

        // Calculate and display final statistics
        printf("\n");
        double total_wait_time = 0;
        double total_turnaround_time = 0;
        
        for (int i = 0; i < process_count; i++) {
            total_wait_time += process_list[i].wait_time;
            total_turnaround_time += process_list[i].turnaround_time;
            
            printf("%s\n", process_list[i].name);
            printf("\tWaiting time:           %d\n", process_list[i].wait_time);
            printf("\tTurnaround time:        %d\n", process_list[i].turnaround_time);
            printf("\n");
        }
        
        printf("Total average waiting time:     %.1f\n", total_wait_time / process_count);
        printf("Total average turnaround time:  %.1f\n", total_turnaround_time / process_count);

        free(remaining_burst_time);
        return 0;
    }

    // Invalid algorithm parameter
    else {
        printf("Invalid algorithm parameter!\n");
        return 1;
    }
}

