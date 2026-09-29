/*
### Exercise 1: The Doubly-Linked Process List (Bidirectional Traversal)
* **Concept:** Pointers to pointers, backward traversal, robust pointer rewiring.
* **The Task:** Modify your `Process` struct to add a `struct Process *prev` pointer. Then, rewrite or create new versions of:
  * `Process *add_process_doubly(Process *head, int pid, const char *name);`
  * `void print_processes_reverse(Process *tail);` (Hint: you'll need a helper function or logic to find the tail node first).
  * `Process *remove_process_doubly(Process *head, int pid);` (Ensuring both `prev` and `next` pointers are properly updated for neighbors).
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Process {
	int pid;
	char name[32];
	struct Process *next;
    struct Process *prev;
} Process;

Process *add_process_doubly(Process *head, int pid, const char *name) {
	
	Process *new_process = malloc(sizeof(Process));
	if (new_process == NULL) {
		return head; // Safety check in case of malloc failure
	}

	new_process->pid = pid;
	strncpy(new_process->name, name, sizeof(new_process->name) - 1);
	new_process->name[sizeof(new_process->name) - 1] = '\0'; // Ensure name is null terminated
	new_process->next = NULL;
    new_process->prev = NULL;

	if (head == NULL) {
		return new_process;
	}

	Process *current_node = head;
    // Loop to reach the tail
	while (current_node->next != NULL) {
        current_node = current_node->next;
	}
	current_node->next = new_process;
    new_process->prev = current_node;

	return head;
}

void print_processes(const Process *head) {
	
	if (head == NULL) {
		printf("There are no processes in the tracker\n");
		return;
	}
	
	printf("Processes in the tracker:\n");
	const Process *current_node = head;
	while (current_node != NULL) {
		printf("PID: %-10d Name: %s\n", current_node->pid, current_node->name);
		current_node = current_node->next;
	}

}

void print_processes_reverse(const Process *tail) {
    
    if (tail == NULL) {
        printf("There are no processes in the tracker\n");
        return;
    }

    printf("Processes in the tracker, reversed:\n");
    const Process *current_node = tail;
    while (current_node != NULL) {
        printf("PID: %-10d Name: %s\n", current_node->pid, current_node->name);
        current_node = current_node->prev;
    }
}

Process *remove_process_doubly(Process *head, int pid) {
	
	if (head == NULL) {
		return head;
	}

	if (head->pid == pid) {
		Process* new_head = head->next;
        if (new_head != NULL) {
            new_head->prev = NULL;
        }
		free(head);
		return new_head;
	}

	Process* current_node = head;
	Process* previous_node = head;
	while (current_node != NULL) {
		if (current_node->pid == pid) {
			previous_node->next = current_node->next;
            if (current_node->next != NULL) {
                current_node->next->prev = previous_node;
            }
			free(current_node);
			return head;
		}
		previous_node = current_node;
		current_node = current_node->next;
	}

	return head;
}

void free_list(Process *head) {

	if (head == NULL) {
		return;
	}

	Process* current_node = head;
	Process* next_node;
	while (current_node != NULL) {
		next_node = current_node->next;
		free(current_node);
		current_node = next_node;
	}

}

int main() {

	Process *head = NULL;

    // 1. Add processes
   	head = add_process_doubly(head, 101, "init");
    head = add_process_doubly(head, 204, "bash");
    head = add_process_doubly(head, 309, "sshguard");

    print_processes(head);

    // 2. Remove a process
    printf("\nRemoving PID 204 (bash)...\n");
    head = remove_process_doubly(head, 204);

    print_processes(head);
    
    // Find tail
    Process *current_node = head;
    while (current_node->next != NULL) {
        current_node = current_node->next;
    }
    print_processes_reverse(current_node);

    // 3. Clean up memory
    free_list(head);
    head = NULL; // Good practice to prevent dangling head pointer
	
	return 0;
}
