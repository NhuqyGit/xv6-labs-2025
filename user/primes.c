#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ  0
#define WRITE 1

void gen_numbers(int write_fd) {
    for (int i = 2; i <= 280; i++) {
        write(write_fd, &i, sizeof(i));
    }
    close(write_fd);
}

void primes_num(int read_fd, int write_fd, int prime) {
    int num;

    while (read(read_fd, &num, sizeof(num)) > 0) {
        if (num % prime != 0) {
            write(write_fd, &num, sizeof(num));
        }
    }

    close(read_fd);
    close(write_fd);
}

int main() {
    int p[2];
    int prime;

    if (pipe(p) < 0) {
        printf("Error creating pipe\n");
        exit(1);
    }

    if (fork() == 0) {
        close(p[READ]);
        gen_numbers(p[WRITE]);
        exit(0);
    }

    close(p[WRITE]);

    while (read(p[READ], &prime, sizeof(prime)) > 0) {
        printf("prime %d\n", prime);

        int new_pipe[2];
        if (pipe(new_pipe) < 0) {
            printf("Error creating new pipe\n");
            exit(1);
        }

        if (fork() == 0) {
            close(new_pipe[READ]);
            primes_num(p[READ], new_pipe[WRITE], prime);
            exit(0);
        }

        close(p[READ]);
        close(new_pipe[WRITE]);
        p[READ] = new_pipe[READ];
    }

    close(p[READ]);

    while (wait(0) > 0);

    exit(0);
}
