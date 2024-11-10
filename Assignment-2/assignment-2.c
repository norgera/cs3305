/**
 * Large Number Multiplication Using Process Communication
 * 
 * This program implements a multiplication algorithm for 4-digit numbers
 * using parent-child process communication through pipes. It splits the
 * numbers and distributes calculations between processes.
 * 
 * Algorithm: For numbers ABCD * EFGH, splits into:
 * - X = (AB * EF) * 10000
 * - Y = (AB * GH + CD * EF) * 100
 * - Z = CD * GH
 * Final result = X + Y + Z
 * 
 * Written by: Nathan Orgera (251175297)
 * Date: Feb 13th, 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Constants
#define MIN_DIGITS 1000
#define MAX_DIGITS 9999
#define REQUIRED_ARGS 3
#define PIPE_SIZE 4
#define MULTIPLIER_X 10000
#define MULTIPLIER_Y 100

// Function prototypes
int atoi(const char *str);

// Pipe file descriptors
// fd[0] is read end, fd[1] is write end
int fd[2];   // Child to parent pipe
int fd1[2];  // Parent to child pipe

/**
 * Main function - Handles process creation and communication
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return 0 on success, non-zero on failure
 */
int main(int argc, char *argv[]) {
    // Process identification
    pid_t pid;

    // Input validation
    if (argc != REQUIRED_ARGS) {
        fprintf(stderr, "Usage: %s <first_number> <second_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse input numbers
    int num_first = atoi(argv[1]);
    int num_second = atoi(argv[2]);

    // Validate input range
    if (num_first < MIN_DIGITS || num_first > MAX_DIGITS || 
        num_second < MIN_DIGITS || num_second > MAX_DIGITS) {
        fprintf(stderr, "Error: Numbers must be 4 digits (1000-9999)\n");
        exit(EXIT_FAILURE);
    }

    // Split numbers into pairs
    int first_high = num_first / 100;    // AB part of ABCD
    int first_low = num_first % 100;     // CD part of ABCD
    int second_high = num_second / 100;  // EF part of EFGH
    int second_low = num_second % 100;   // GH part of EFGH

    // Variables for partial results
    int partial_result_x;  // Result for X calculation
    int partial_result_y;  // Result for Y calculation
    int partial_result_z;  // Result for Z calculation
    int final_result;      // Final multiplication result

    // Variables for pipe communication
    int pipe_result;      // Results passed through pipes
    int operand_first;    // First operand in multiplication
    int operand_second;   // Second operand in multiplication

    // Create pipes
    if (pipe(fd) < 0 || pipe(fd1) < 0) {
        fprintf(stderr, "Error: Pipe creation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("\nYour integers are %d %d\n", num_first, num_second);

    // Create child process
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: Fork failed\n");
        exit(EXIT_FAILURE);
    }

    // Parent process
    if (pid > 0) {
        close(fd[1]);   // Close unused write end
        close(fd1[0]);  // Close unused read end

        printf("Parent (PID %d): Created child (PID %d)\n", getpid(), pid);

        // Calculate X = (AB * EF) * 10000
        printf("\n###\n# Calculating X\n###\n");
        
        // Send AB and EF to child
        printf("Parent (PID %d): Sending %d to child\n", getpid(), first_high);
        write(fd1[1], &first_high, PIPE_SIZE);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), second_high);
        write(fd1[1], &second_high, PIPE_SIZE);
        
        // Receive result from child
        read(fd[0], &pipe_result, PIPE_SIZE);
        printf("Parent (PID %d): Received %d from child\n", getpid(), pipe_result);
        partial_result_x = pipe_result * MULTIPLIER_X;

        // Calculate Y = (AB * GH + CD * EF) * 100
        printf("\n###\n# Calculating Y\n###\n");
        
        // First part: AB * GH
        printf("Parent (PID %d): Sending %d to child\n", getpid(), first_high);
        write(fd1[1], &first_high, PIPE_SIZE);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), second_low);
        write(fd1[1], &second_low, PIPE_SIZE);
        
        read(fd[0], &pipe_result, PIPE_SIZE);
        printf("Parent (PID %d): Received %d from child\n", getpid(), pipe_result);
        partial_result_y = pipe_result;

        // Second part: CD * EF
        printf("Parent (PID %d): Sending %d to child\n", getpid(), first_low);
        write(fd1[1], &first_low, PIPE_SIZE);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), second_high);
        write(fd1[1], &second_high, PIPE_SIZE);
        
        read(fd[0], &pipe_result, PIPE_SIZE);
        printf("Parent (PID %d): Received %d from child\n", getpid(), pipe_result);
        partial_result_y = (partial_result_y + pipe_result) * MULTIPLIER_Y;

        // Calculate Z = CD * GH
        printf("\n###\n# Calculating Z\n###\n");
        
        printf("Parent (PID %d): Sending %d to child\n", getpid(), first_low);
        write(fd1[1], &first_low, PIPE_SIZE);
        printf("Parent (PID %d): Sending %d to child\n", getpid(), second_low);
        write(fd1[1], &second_low, PIPE_SIZE);
        
        read(fd[0], &pipe_result, PIPE_SIZE);
        printf("Parent (PID %d): Received %d from child\n", getpid(), pipe_result);
        partial_result_z = pipe_result;

        // Calculate and display final result
        final_result = partial_result_x + partial_result_y + partial_result_z;
        printf("\n%d*%d == %d + %d + %d === %d\n", 
               num_first, num_second, 
               partial_result_x, partial_result_y, partial_result_z, 
               final_result);

    // Child process
    } else {
        close(fd[0]);   // Close unused read end
        close(fd1[1]);  // Close unused write end

        while (1) {
            // Read operands from parent
            read(fd1[0], &operand_first, PIPE_SIZE);
            printf("      Child (PID %d): Received %d from parent\n", 
                   getpid(), operand_first);
            read(fd1[0], &operand_second, PIPE_SIZE);
            printf("      Child (PID %d): Received %d from parent\n", 
                   getpid(), operand_second);

            // Calculate result and send to parent
            pipe_result = operand_first * operand_second;
            printf("      Child (PID %d): Sending %d to parent\n", 
                   getpid(), pipe_result);
            write(fd[1], &pipe_result, PIPE_SIZE);
        }
    }

    return EXIT_SUCCESS;
}