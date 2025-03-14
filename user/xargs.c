#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARG 10  // Max argument
#define BUFSIZE 512  // Buff size

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs [-n num] command [args...]\n");
        exit(1);
    }

    int n_arg = 0;
    int cmd_start = 1;

    // Check if the -n option is present
    if (argc > 2 && strcmp(argv[1], "-n") == 0) {
        n_arg = atoi(argv[2]);  // Convert string to int
        cmd_start = 3;  // skip "-n num"
        if (n_arg <= 0) {
            fprintf(2, "xargs: invalid value for -n\n");
            exit(1);
        }
    }

    char *cmd[MAXARG];
    int i;
    for (i = cmd_start; i < argc; i++) {
        cmd[i - cmd_start] = argv[i];
    }

    char buf[BUFSIZE];
    int n = 0;
    
    while (read(0, buf + n, 1) == 1) {
        if (buf[n] == '\n') {
            buf[n] = 0;  // Add a character to the end of the string
            cmd[i - cmd_start] = buf;  // Add new parameters to the command
            cmd[i - cmd_start + 1] = 0;

            if (fork() == 0) {
                exec(cmd[0], cmd);  // Execute command
                fprintf(2, "exec %s failed\n", cmd[0]);
                exit(1);
            }

            wait(0);
            n = 0;  // Reset buff
        } else {
            n++;
        }
    }

    exit(0);
}
