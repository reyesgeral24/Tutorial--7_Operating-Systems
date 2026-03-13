#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork(); // Fork a child process

    if (pid == -1)
    {
        // Fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process: execute "process" using execl
        execl("./process", "process", NULL);

        // If execl returns, it failed
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process:
        sleep(5); // Wait 5 seconds while child runs

        kill(pid, SIGINT); // Send SIGINT to the child process

        waitpid(pid, NULL, 0); // Wait for child to fully terminate

        printf("Process terminated.\n");
    }

    return 0;
}
