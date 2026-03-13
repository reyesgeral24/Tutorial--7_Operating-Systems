#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // child process
        execl("./printer", "./printer", NULL);
        perror("exec failed");
        exit(1);
    }

    else {
        printf("Parent created child with PID %d\n", pid);

        sleep(5);

        printf("Parent sending SIGTSTP\n");
        kill(pid, SIGTSTP);

        sleep(10);

        printf("Parent sending SIGCONT\n");
        kill(pid, SIGCONT);

        waitpid(pid, NULL, 0);

        printf("Child finished. Parent exiting.\n");
    }

    return 0;
}