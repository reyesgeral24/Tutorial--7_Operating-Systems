#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for process
typedef struct {
    char name[256];
    int priority;
    int pid;
    int runtime;
} proc;

// Linked list node
typedef struct queue {
    proc process;
    struct queue *next;
} queue;

// Head of the linked list
queue *head = NULL;

// Push function to add process to queue
void push(proc process) {
    queue *newNode = (queue *)malloc(sizeof(queue));
    
    newNode->process = process;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    } else {
        queue *temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}

int main() {
    FILE *file;
    char line[512];

    file = fopen("processes.txt", "r");

    if (file == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    // Read 10 lines from file
    while (fgets(line, sizeof(line), file) != NULL) {
        proc p;

        // Remove newline if present
        line[strcspn(line, "\n")] = 0;

        // Parse CSV
        char *token;

        token = strtok(line, ",");
        strcpy(p.name, token);

        token = strtok(NULL, ",");
        p.priority = atoi(token);

        token = strtok(NULL, ",");
        p.pid = atoi(token);

        token = strtok(NULL, ",");
        p.runtime = atoi(token);

        // Add to linked list
        push(p);
    }

    fclose(file);

    // Iterate through the linked list and print
    queue *temp = head;

    printf("Processes in Queue:\n\n");

    while (temp != NULL) {
        printf("Name: %s\n", temp->process.name);
        printf("Priority: %d\n", temp->process.priority);
        printf("PID: %d\n", temp->process.pid);
        printf("Runtime: %d seconds\n\n", temp->process.runtime);

        temp = temp->next;
    }

    return 0;
}
