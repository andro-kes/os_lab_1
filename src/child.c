#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "child.h"

// Convert int to string, returns length
int int_to_str(int n, char *buf) {
    int i = 0;
    int is_negative = 0;
    if (n < 0) { is_negative = 1; n = -n; }
    if (n == 0) { buf[i++] = '0'; return i; }

    char tmp[20];
    int j = 0;
    while (n > 0) { tmp[j++] = '0' + (n % 10); n /= 10; }
    if (is_negative) buf[i++] = '-';
    while (j > 0) buf[i++] = tmp[--j];
    return i;
}

// Check if string is a number
int is_number(const char *s) {
    if (*s == '-' || *s == '+') s++;
    if (!*s) return 0;
    while (*s) { if (!isdigit(*s)) return 0; s++; }
    return 1;
}

// Check if number is prime
int is_prime(int n) {
    if (n <= 1) return 0;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return 0;
    return 1;
}

// Child process: read from pipe, check numbers, write results to parent
void run_child(int read_fd, int write_fd) {
    char line[256];
    int idx = 0;
    char buf;
    ssize_t n;

    const char *composite_msg = " - composite.\n";
    const char *prime_msg = " - prime or negative. Terminating.\n";
    const char *skip_msg1 = "Skipping invalid line: '";
    const char *skip_msg2 = "'\n";

    while ((n = read(read_fd, &buf, 1)) > 0) {
        if (buf == '\n' || buf == '\r') {
            line[idx] = '\0';
            if (idx > 0) {
                if (!is_number(line)) {
                    write(write_fd, skip_msg1, strlen(skip_msg1));
                    write(write_fd, line, idx);
                    write(write_fd, skip_msg2, strlen(skip_msg2));
                } else {
                    int num = atoi(line);
                    write(write_fd, "Number ", strlen("Number "));
                    char num_buf[20];
                    int len = int_to_str(num, num_buf);
                    write(write_fd, num_buf, len);
                    if (num <= 1 || is_prime(num)) {
                        write(write_fd, prime_msg, strlen(prime_msg));
                        _exit(0);
                    } else {
                        write(write_fd, composite_msg, strlen(composite_msg));
                    }
                }
            }
            idx = 0;
        } else if (idx < sizeof(line) - 1) {
            line[idx++] = buf;
        }
    }

    // Last line without newline
    if (idx > 0) {
        line[idx] = '\0';
        if (!is_number(line)) {
            write(write_fd, skip_msg1, strlen(skip_msg1));
            write(write_fd, line, idx);
            write(write_fd, skip_msg2, strlen(skip_msg2));
        } else {
            int num = atoi(line);
            write(write_fd, "Number ", strlen("Number "));
            char num_buf[20];
            int len = int_to_str(num, num_buf);
            write(write_fd, num_buf, len);
            if (num <= 1 || is_prime(num)) {
                write(write_fd, prime_msg, strlen(prime_msg));
                _exit(0);
            } else {
                write(write_fd, composite_msg, strlen(composite_msg));
            }
        }
    }

    _exit(0);
}