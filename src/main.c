#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "errno.h"
#include "stdint.h"
#include "sys/wait.h"

#define _R_            "\x1b[31m" // red color for child process output color
#define _G_            "\x1b[32m" // green color for parent process output color
#define _S_            "\x1b[33m" // green color for parent process output color
#define RESET_COLOR    "\x1b[0m"  // reset color

int checkSimple(long long n) {
    if (n <= 2) {
        return 1; 
    }
    if (n % 2 == 0) {
        return 0;
    }
    
    for (long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return 0; // good
        }
    }
    return 1; // not good
}

int main() {
    int pd[2], cd[2]; // pd - parent to child, cd - child to parent
    
    printf("%s", "Введите имя файла:\n");
    char filename[256];
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Failed to read filename\n");
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Remove \r\n (thank u windows) or \n
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len > 0 && line[len - 1] == '\r') {
            line[len - 1] = '\0';
        }
        // Skip empty lines
        if (line[0] == '\0') {
            continue;
        }

        errno = 0;
        char *endptr = NULL;
        long long value = strtoll(line, &endptr, 10);
        
        if (errno != 0 || endptr == line || *endptr != '\0') {
            // Skip
            fprintf(stderr, _S_"Skip invalid line: '%s'\n", line);
            continue;
        }
        
        // Create pipes
        if (pipe(pd) == -1 || pipe(cd) == -1) { 
            perror("pipe"); 
            return -1; 
        }
        
        // Create new proccess
        pid_t pid = fork();
        if (pid == -1) { 
            perror("fork"); 
            return -1;
        }
        
        if (pid == 0) {
            // Child process
            printf(_R_"Child process %d processing number: %lld\n"RESET_COLOR, getpid(), value);
            
            // Закрываем ненужные концы пайпов
            close(pd[1]); // close pd write
            close(cd[0]); // close cd read
            
            // Read
            long long received_value;
            read(pd[0], &received_value, sizeof(received_value));
            close(pd[0]); // close pd read
            
            int result = checkSimple(received_value);
            
            if (result == 1) {
                // Not good
                printf(_R_"Number %lld is prime or negative. Terminating both processes.\n"RESET_COLOR, received_value);
                close(cd[1]); // close cd write
                exit(1);
            } else {
                // Good
                write(cd[1], &received_value, sizeof(received_value));
                close(cd[1]); // close cd write
                exit(0);
            }
        } else {
            // Parent process
            printf(_G_"Parent created child process %d for number: %lld\n"RESET_COLOR, pid, value);
            
            close(pd[0]); // close pd read
            close(cd[1]); // close cd write
            
            write(pd[1], &value, sizeof(value));
            close(pd[1]); // close pd write
            
            int status;
            waitpid(pid, &status, 0);
            
            // Check status
            if (WEXITSTATUS(status) == 1) {
                // Not good
                printf(_G_"Child process terminated. Parent also terminating.\n"RESET_COLOR);
                close(cd[0]); // close cd read
                fclose(fp);
                return 0; 
            } else {
                // Good
                long long result_value;
                read(cd[0], &result_value, sizeof(result_value));
                close(cd[0]); // close cd read
                
                printf("%lld\n", result_value);
            }
        }
    }
    fclose(fp);

    return 0;
}