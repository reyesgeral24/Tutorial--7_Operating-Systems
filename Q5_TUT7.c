#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
typedef struct proc {
    char name[256];
    int priority;
    int pid;
    int runtime;
} proc;

typedef struct queue_node {
    proc process;
    struct queue_node *next;
} queue_node;

static queue_node *queue = NULL;

/* Add process to end of queue */
void push(proc process) {
    queue_node *new_node = malloc(sizeof(queue_node));
    if (new_node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    new_node->process = process;
    new_node->next = NULL;

    if (queue == NULL) {
        queue = new_node;
        return;
    }

    queue_node *current = queue;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

/* Remove first process from queue */
proc pop(void) {
    proc empty_proc;
    memset(&empty_proc, 0, sizeof(proc));
    strcpy(empty_proc.name, "");

    if (queue == NULL) {
        return empty_proc;
    }

    queue_node *temp = queue;
    proc result = temp->process;
    queue = queue->next;
    free(temp);

    return result;
}

/* Delete first process with matching name */
proc delete_name(char *name) {
    proc empty_proc;
    memset(&empty_proc, 0, sizeof(proc));
    strcpy(empty_proc.name, "");

    if (queue == NULL) {
        return empty_proc;
    }

    queue_node *current = queue;
    queue_node *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->process.name, name) == 0) {
            proc result = current->process;

            if (previous == NULL) {
                queue = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return result;
        }

        previous = current;
        current = current->next;
    }

    return empty_proc;
}

/* Check whether queue is empty */
bool is_empty(void) {
    return queue == NULL;
}

/* Run one process for its runtime, terminate with SIGINT, wait, then print */
void execute_process(proc *p) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        /* child */
        char runtime_str[32];
        snprintf(runtime_str, sizeof(runtime_str), "%d", p->runtime);

        execl("./process", "process", runtime_str, (char *)NULL);

        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        /* parent */
        p->pid = (int)pid;

        sleep(p->runtime);

        if (kill(pid, SIGINT) == -1) {
            perror("kill");
        }

        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
        }

        printf("Name: %s, Priority: %d, PID: %d, Runtime: %d\n",
               p->name, p->priority, p->pid, p->runtime);
    }
}

int main(void) {
    FILE *file = fopen("processes_q5.txt", "r");
    if (file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    char line[512];

    /* Read file: name, priority, runtime */
    while (fgets(line, sizeof(line), file) != NULL) {
        proc p;
        memset(&p, 0, sizeof(proc));

        if (sscanf(line, " %255[^,], %d, %d", p.name, &p.priority, &p.runtime) == 3) {
            p.pid = 0;   /* must start at 0 */
            push(p);
        }
    }

    fclose(file);

    /* Execute all priority 0 processes first */
    while (1) {
        queue_node *current = queue;
        bool found_priority_zero = false;
        char name_to_delete[256];

        while (current != NULL) {
            if (current->process.priority == 0) {
                strcpy(name_to_delete, current->process.name);
                found_priority_zero = true;
                break;
            }
            current = current->next;
        }

        if (!found_priority_zero) {
            break;
        }

        proc p = delete_name(name_to_delete);
        if (strlen(p.name) > 0) {
            execute_process(&p);
        }
    }

    /* Execute remaining processes in FIFO order using pop() */
    while (!is_empty()) {
        proc p = pop();
        if (strlen(p.name) > 0) {
            execute_process(&p);
        }
    }

    return 0;
}