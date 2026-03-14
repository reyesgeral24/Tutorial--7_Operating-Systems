#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int sig) {
    printf("Process received SIGINT. Terminating...\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handler);

    while (1) {
        printf("Process running...\n");
        sleep(1);
    }

    return 0;
}
