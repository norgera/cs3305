/*
 * Password cracking program to hack the file "hackme".
 * This program demonstrates password cracking methods with and without forking
 * to compare computational efficiency.
 *
 * Written by: Nathan Orgera (251175297)
 * Date: Jan 31st, 2023
 */

// Required libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "checkPassword.h"
#include <errno.h>

/**
 * Brute force password cracking function.
 * Utilizes the checkPassword function to crack a password segment of length 3.
 * Uses nested loops to test all printable ASCII characters (33-126).
 */
void crack_password(int start) {
    char password_attempt[4] = {0};  // 3 chars + null terminator

    // Iterate through all printable ASCII characters for each position
    for (int char1 = 33; char1 <= 126; char1++) {
        password_attempt[0] = (char)char1;

        for (int char2 = 33; char2 <= 126; char2++) {
            password_attempt[1] = (char)char2;

            for (int char3 = 33; char3 <= 126; char3++) {
                password_attempt[2] = (char)char3;

                if (checkPassword(password_attempt, start) == 0) {
                    printf("%s\n", password_attempt);
                    fflush(stdout);
                    return;
                }
            }
        }
    }
    printf("No valid password found in ASCII range\n");
}

/**
 * Main function - Controls program execution flow and process management
 * Accepts command line arguments to enable forking functionality
 */
int main(int argc, char *argv[]) {
    // Process ID variables for parent and child processes
    pid_t child1_pid, child2_pid, process_pid, parent_pid;
    
    int enable_fork = 0;  // Flag to control forking behavior

    // Check command line arguments for fork enablement
    if (argc > 1 && strcmp(argv[1], "-f") == 0) {
        enable_fork = 1;
    }

    // Without forking
    if (enable_fork == 0) {
        crack_password(0);
        crack_password(3);
        crack_password(6);
        crack_password(9);
        return 0;
    }

    // With forking
    child1_pid = fork();
    child2_pid = fork();

    if (child1_pid == 0) {
        if (child2_pid == 0) {
            // Process 1.1.1 (grandchild)
            process_pid = getpid();
            parent_pid = getppid();
            printf("PID 1.1 IS %d. Child 1.1.1 IS %d\n", parent_pid, process_pid);
            printf("PID 1.1.1 IS %d. PPID 1.1 IS %d\n", process_pid, parent_pid);
            printf("PID 1.2 IS %d. PPID 1.0 IS %d\n", parent_pid + 1, getppid() - 1);
            fflush(stdout);
            crack_password(9);

        } else {
            // Process 1.1 (first child)
            crack_password(6);
            wait(NULL);  // Wait for grandchild completion
        }
    } else {
        if (child2_pid == 0) {
            // Process 1.2 (second child)
            crack_password(3);
            wait(NULL);  // Wait for child completion

        } else {
            // Process 1.0 (parent)
            printf("PID 1.0 IS %d. Child 1.1 IS %d\n", getpid(), child1_pid);
            printf("PID 1.1 IS %d. PPID IS 1.0 %d\n", child1_pid, getpid());
            printf("PID 1.0 IS %d. Child 1.2 IS %d\n", getpid(), child2_pid);
            fflush(stdout);
            crack_password(0);
            wait(NULL);  // Wait for all child processes
            wait(NULL);
        }
    }
    return 0;
}
