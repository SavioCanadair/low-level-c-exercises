/* 
### Exercise 5: Binary File Parser (Binary Serialization)
* **Concept:** Struct padding, binary I/O (`fread`/`fwrite`), reading structured data directly from disk files.
* **The Task:** Write a program that takes your linked list of processes and serializes (saves) them directly into a binary file on disk.
  * `void save_to_disk(const Process *head, const char *filename);`
  * `Process *load_from_disk(const char *filename);`
  * *Security Bonus consideration:* Be mindful of endianness and struct padding when writing raw binary structures to disk!
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

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

void save_to_disk(const Process *head, const char *filename) {
    
    if (head == NULL) {
        return;
    }

    FILE *handle = fopen(filename, "wb");
    if (handle == NULL) {
        printf("Error opening file!\n");
        return;
    }

    const Process *current_node = head;
    size_t ret;
    while (current_node != NULL) {
        // Write PID
        ret = fwrite(&(current_node->pid), sizeof(current_node->pid), 1, handle);
        if (ret != 1) {
            printf("Writing PID failed!\n");
            fclose(handle);
            return;
        }
        // Write name
        ret = fwrite(current_node->name, sizeof(char), 32, handle);
        if (ret != 32) {
            printf("Writing name failed!\n");
            fclose(handle);
            return;
        }

        current_node = current_node->next;
    }

    fclose(handle);

}

Process *load_from_disk(const char *filename) {
    
    Process *head = NULL;

    FILE *handle = fopen(filename, "rb");
    if (handle == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }
    
    size_t ret;
    int pid;
    char name[32];
    
    while (1) {
        
        // Read PID
        ret = fread(&pid, sizeof(int), 1, handle);
        if (ret != 1) {
            fclose(handle);
            return head;
        }
        // Read name
        ret = fread(name, sizeof(char), 32, handle);
        if (ret != 32) {
            printf("Reading process name failed!\n");
            fclose(handle);
            return head;
        }

        head = add_process(head, pid, name);
    }

}

int main() {
    
    Process *head = NULL;

    // 1. Add processes
    head = add_process(head, 101, "init");
    head = add_process(head, 204, "bash");
    head = add_process(head, 309, "sshguard");

    printf("--- Original Processes --- \n");
    print_processes(head);

    // 2. Save the linked list to a binary file on disk
    printf("\nSaving processes to 'processes.bin'...\n");
    save_to_disk(head, "processes.bin");

    // 3. Clean up memory to simulate restarting
    printf("\nFreeing memory...\n");
    free_list(head);
    head = NULL;

    // 4. Load the linked list back from the binary file
    printf("\nLoading processes from 'processes.bin'...\n");
    head = load_from_disk("processes.bin");

    printf("--- Loaded Processes --- \n");
    print_processes(head);

    // 5. Automated Assertions to test output integrity
    // (Note: Adjust order/checks based on whether your add_process prepends or appends)
    assert(head != NULL);

    // Example: checking the first node in the loaded list
    // (Assuming add_process appends; if it prepends, the expected PIDs will be reversed)
    assert(head->pid == 101);

    // If you want to check deeper nodes:
    if (head->next != NULL) {
        assert(head->next->pid == 204);
    }

    printf("\nAll assertions passed successfully! Serialization works correctly.\n");

    // 6. Final clean up
    free_list(head);
    head = NULL;

    return 0;
}
