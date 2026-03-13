#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

typedef struct {
    char name[256];
    int priority;
    int pid;
    int runtime;
} proc;

typedef struct queue {
    proc process;
    struct queue *next;
} queue;

queue *head = NULL;

void push(proc process)
{
    queue *newNode = malloc(sizeof(queue));
    newNode->process = process;
    newNode->next = NULL;

    if (head == NULL)
    {
        head = newNode;
        return;
    }

    queue *temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = newNode;
}

proc pop()
{
    proc empty;
    strcpy(empty.name, "NULL");
    empty.priority = -1;
    empty.pid = -1;
    empty.runtime = -1;

    if (head == NULL)
        return empty;

    queue *temp = head;
    proc removed = temp->process;

    head = head->next;
    free(temp);

    return removed;
}

proc delete_name(char *name)
{
    proc empty;
    strcpy(empty.name, "NULL");
    empty.pid = -1;

    queue *temp = head;
    queue *prev = NULL;

    while (temp != NULL)
    {
        if (strcmp(temp->process.name, name) == 0)
        {
            if (prev == NULL)
                head = temp->next;
            else
                prev->next = temp->next;

            proc removed = temp->process;
            free(temp);
            return removed;
        }

        prev = temp;
        temp = temp->next;
    }

    return empty;
}

proc delete_pid(int pid)
{
    proc empty;
    strcpy(empty.name, "NULL");
    empty.pid = -1;

    queue *temp = head;
    queue *prev = NULL;

    while (temp != NULL)
    {
        if (temp->process.pid == pid)
        {
            if (prev == NULL)
                head = temp->next;
            else
                prev->next = temp->next;

            proc removed = temp->process;
            free(temp);
            return removed;
        }

        prev = temp;
        temp = temp->next;
    }

    return empty;
}

int main(void)
{
    FILE *file = fopen("processes.txt", "r");

    if (file == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    char line[512];

    while (fgets(line, sizeof(line), file))
    {
        proc p;
        char *token;

        line[strcspn(line, "\n")] = 0;

        token = strtok(line, ",");
        strcpy(p.name, token);

        token = strtok(NULL, ",");
        p.priority = atoi(token);

        token = strtok(NULL, ",");
        p.pid = atoi(token);

        token = strtok(NULL, ",");
        p.runtime = atoi(token);

        push(p);
    }

    fclose(file);

    // required operations
    delete_name("emacs");
    delete_pid(12235);

    printf("Remaining processes:\n\n");

    while (head != NULL)
    {
        proc p = pop();

        printf("Name: %s\n", p.name);
        printf("Priority: %d\n", p.priority);
        printf("PID: %d\n", p.pid);
        printf("Runtime: %d seconds\n\n", p.runtime);
    }

    return 0;
}
