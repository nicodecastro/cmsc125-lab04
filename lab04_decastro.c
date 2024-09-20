/*
 * Author: John Nico T. De Castro
 * Section: C-5L
 * Program Description: A program that simulates SRPT CPU scheduling.
 */

#include <stdio.h>
#include <stdlib.h>
#define MAX 10

/* Process Control Block (PCB) */
typedef struct PCB {
	int process_id;
	int arrival_time;
	int job_length;
	int wait_time;
	int run_time;
}PCB;

/* Singly-linked list of PCB */
typedef struct linked_list{
	PCB pcb;
	struct linked_list *next;
}linked_list;

/* Stores the processes */
linked_list *ready_queue = NULL;


/* Prints the details of the process */
void PrintPCB(PCB pcb){
	printf("\tProcess ID: %d, Arrival Time: %d, Job Length: %d, Wait Time: %d, Run Time: %d\n", pcb.process_id, pcb.arrival_time, pcb.job_length, pcb.wait_time, pcb.run_time);
}


/* Prints the processes in the ready queue */
void PrintReadyQueue(){
	linked_list *ptr = ready_queue;

	printf("\n\tPrinting Ready Queue:\n");

	while(ptr!=NULL){
		PrintPCB(ptr->pcb);
		ptr = ptr->next;
	}
}


/* 
 * Reads from process.txt
   * First line of the file is the process count
   * Succeeding lines are the <arrival_time> <job_length> values
   * Assume that the processes are sorted in ascending order by arrival time
 * Stores <arrival_time> to arrival_time array
 * Stores <job_length> to job_length array
 * Returns the process count
 */
int ReadProcesses(int *arrival_time, int *job_length){
	int arrival, length, pcnt;
	FILE *fp;

	fp = fopen("process.txt", "rw");
	
	if(fp != NULL){
		fscanf(fp, "%d", &pcnt);
		for (int i = 0; i < pcnt; i++) {
			fscanf(fp, "%d", &arrival);
			fscanf(fp, "%d", &length);
			// printf("%d %d\n", arrival, length);		// for testing purposes
			arrival_time[i] = arrival;
			job_length[i] = length;
		}

		fclose(fp);
	}
	return pcnt;
}


/* 
 * Prints the terminated process
 * Removes the terminated process at the head of the queue
 */
void Dequeue(){	
	printf("\tThe following process already ended:\n");
	
	linked_list **ptr = &ready_queue;

	PrintPCB((*ptr)->pcb);
	*ptr = (*ptr)->next;
}


/* Inserts the process in the queue */
void Enqueue(int process_id, int arrival, int job_length){
	linked_list *new_linked_list = (linked_list *) malloc(sizeof(linked_list));
	new_linked_list->pcb = *((PCB *) malloc(sizeof(PCB)));
	new_linked_list->pcb.process_id = process_id;
	new_linked_list->pcb.arrival_time = arrival;
	new_linked_list->pcb.job_length = job_length;
	new_linked_list->pcb.wait_time = 0;
	new_linked_list->pcb.run_time = 0;
	new_linked_list->next = NULL;

	linked_list **ptr = &ready_queue;
	if (*ptr == NULL) {
		(*ptr) = new_linked_list;
	} else {
		// Check if new job length < remaining time of running process, if true set as new running process (head of queue)
		if (job_length < ((*ptr)->pcb.job_length - (*ptr)->pcb.run_time)) {
			new_linked_list->next = (*ptr);
			(*ptr) = new_linked_list;
		// Do the same, but for the remaining time of queued processes, insert the new process before queued process where new job length < remaining time
		} else {
			linked_list *temp = *ptr;
			while (temp->next != NULL && job_length > (temp->next->pcb.job_length - temp->next->pcb.run_time)) {	// stop at end or before queued process where new job len < remaining time of queued process
				temp = temp->next;
			} 
			new_linked_list->next = temp->next;
			temp->next = new_linked_list;			
		}
	}
}


/*
 * Simulates processing of a single CPU cycle
 * Increases the run time of the currently executing process
   * Currently executing processs is the first process in the queue
 * Increases the wait time of the other processes in the queue
 * Checks if a process terminates (run time == job length)
 */
void Processing(){
	linked_list *ptr = ready_queue;
	ptr->pcb.run_time++;
	if(ptr->pcb.run_time == ptr->pcb.job_length){
		Dequeue();
	}
	ptr = ptr->next;
	while(ptr!=NULL){
		ptr->pcb.wait_time++;
		ptr = ptr->next;
	}
}


/* 
 * Simulates the CPU scheduling
 * Checks the arrival of processes in a given cpu cycle
 * Updates the processes in the queue
 * Prints the processes in the ready queue
 */
void RunSimulation(int *arrival_time, int *job_length, int process_count){
	int i=0, cpu_cycles=1;
	
	while(1){
		printf("\nCPU Cycle: %d\n", cpu_cycles);
		
		if(cpu_cycles == arrival_time[i]) {
			Enqueue(i, arrival_time[i], job_length[i]);
			i++;
		}

		Processing();
		PrintReadyQueue();
		cpu_cycles++;
		if(ready_queue == NULL) break;
	}
}


int main(){	
	int arrival_time[MAX], job_length[MAX];
	int process_count = ReadProcesses(arrival_time, job_length);
	RunSimulation(arrival_time, job_length, process_count);
}