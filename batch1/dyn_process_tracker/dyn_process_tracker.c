/* Exercise: The Dynamic Process Tracker (Singly Linked List)
Imagine you are writing a tiny OS utility to track active system processes. You need to implement a linked list where each node is a dynamically allocated struct representing a process.

1. Define Your Struct
Create a struct named Process that contains:
- An integer pid (Process ID)
- A fixed-size string name (e.g., char name[32];)
- A self-referential pointer struct Process *next to point to the next node in the list.

2. Implement These Four Functions
Write a C program with a main function that implements and tests the following functions:
- Process *add_process(Process *head, int pid, const char *name);
   - Allocates memory for a new Process using malloc.
   - Populates its fields.
   - Appends it to the end of the linked list and returns the head.
- void print_processes(const Process *head);
   - Traverses the list and prints out every process's PID and name in a clean format.
- Process *remove_process(Process *head, int pid);
   - Searches the list for a specific pid.
   - If found, safely removes it from the chain and frees its allocated memory to prevent leaks. Returns the head.
- void free_list(Process *head);
   - Traverses the entire list at the end of the program and frees every node.

3. The Low-Level Challenge (Testing)
Once you write it, compile it and run it through Valgrind to prove you have zero memory leaks:
valgrind --leak-check=full ./your_program
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Process {
	int pid;
	char name[32];
	struct Process *next;
} Process;

Process *add_process(Process *head, int pid, const char *name) {
	
	Process *new_process = malloc(sizeof(Process));
	if (new_process == NULL) {
		return head; // Safety check in case of malloc failure
	}

	new_process->pid = pid;
	strncpy(new_process->name, name, sizeof(new_process->name) - 1);
	new_process->name[sizeof(new_process->name) - 1] = '\0'; // Ensure name is null terminated
	new_process->next = NULL;

	if (head == NULL) {
		return new_process;
	}

	Process *current_node = head;
	while (current_node->next != NULL) {
		current_node = current_node->next;
	}
	current_node->next = new_process;

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

Process *remove_process(Process *head, int pid) {
	
	if (head == NULL) {
		return head;
	}

	if (head->pid == pid) {
		Process* new_head = head->next;
		free(head);
		return new_head;
	}

	Process* current_node = head;
	Process* previous_node = head;
	while (current_node != NULL) {
		if (current_node->pid == pid) {
			previous_node->next = current_node->next;
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
    	head = add_process(head, 101, "init");
    	head = add_process(head, 204, "bash");
    	head = add_process(head, 309, "sshguard");

    	print_processes(head);

    	// 2. Remove a process
    	printf("\nRemoving PID 204 (bash)...\n");
    	head = remove_process(head, 204);

    	print_processes(head);

    	// 3. Clean up memory
    	free_list(head);
    	head = NULL; // Good practice to prevent dangling head pointer
	
	return 0;
}
