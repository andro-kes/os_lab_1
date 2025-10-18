#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "child.h"

int main() {
    char filename[256];

    const char *prompt = "Enter input filename:\n";
    write(STDOUT_FILENO, prompt, strlen(prompt));

    ssize_t n = read(STDIN_FILENO, filename, sizeof(filename));
    if (n <= 0) return 1;
    if (filename[n-1] == '\n') filename[n-1] = '\0';

    const char *output_file = "output.txt";

    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];

    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        const char *err = "Pipe creation error\n";
        write(STDERR_FILENO, err, strlen(err));
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        const char *err = "Fork error\n";
        write(STDERR_FILENO, err, strlen(err));
        return 1;
    }

    if (pid == 0) {
        // Child process
        close(pipe_parent_to_child[1]); // close write end
        close(pipe_child_to_parent[0]); // close read end
        run_child(pipe_parent_to_child[0], pipe_child_to_parent[1]);
    } else {
        // Parent process
        close(pipe_parent_to_child[0]); // close read end
        close(pipe_child_to_parent[1]); // close write end

        int in_fd = open(filename, O_RDONLY);
        if (in_fd == -1) {
            const char *err = "Failed to open input file\n";
            write(STDERR_FILENO, err, strlen(err));
            return 1;
        }

        int out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd == -1) {
            const char *err = "Failed to open output.txt\n";
            write(STDERR_FILENO, err, strlen(err));
            return 1;
        }

        // Read input file and send to child
        char buf[1024];
        ssize_t m;
        while ((m = read(in_fd, buf, sizeof(buf))) > 0) {
            write(pipe_parent_to_child[1], buf, m);
        }
        close(pipe_parent_to_child[1]);
        close(in_fd);

        // Read results from child and write to output.txt
        while ((m = read(pipe_child_to_parent[0], buf, sizeof(buf))) > 0) {
            write(out_fd, buf, m);
        }

        close(pipe_child_to_parent[0]);
        close(out_fd);

        waitpid(pid, NULL, 0);
    }

    return 0;
}