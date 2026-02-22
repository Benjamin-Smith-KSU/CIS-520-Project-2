#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) {
  // decrement the burst time of the pcb
  --process_control_block->remaining_burst_time;
}

// static variable for fcfs compare
static dyn_array_t * fcfs_queue = NULL;

// comapre function for fcfs, sorts indicies based on the value of that index in fcfs_queue to avoid sorting the queue in place
int fcfs_compare(const void *x, const void *y) {
	size_t i = *(size_t*)x;
	size_t j = *(size_t*)y;
	ProcessControlBlock_t* block_x = (ProcessControlBlock_t*)dyn_array_at(fcfs_queue, i);
	ProcessControlBlock_t* block_y = (ProcessControlBlock_t*)dyn_array_at(fcfs_queue, j);
	int arrival_x = (int)block_x->arrival;
	int arrival_y = (int)block_y->arrival;
	int res = arrival_x - arrival_y;
	if (res == 0) return (i < j) ? -1 : 1;
	return res;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result) 
{
	if (ready_queue == NULL || result == NULL) return false;
	if (dyn_array_empty(ready_queue) || dyn_array_data_size(ready_queue) == 0 || dyn_array_capacity(ready_queue) == 0) return false;

	float total_waiting_time = 0;
	float total_turnaround_time = 0;
	result.total_run_time = 0;
	const size_t size = dyn_array_size(ready_queue);

	size_t ind[size];
	for (size_t i = 0; i < size; i++) {
		ind[i] = i;
	}

	fcfs_queue = ready_queue;
	qsort(ind, size, sizeof(size_t), fcfs_compare);
	fcfs_queue = NULL;

	for (size_t i = 0; i < size; i++) {

		ProcessControlBlock_t* block = dyn_array_at(ready_queue, ind[i]);
		if (block == NULL) return false;

		if (block->arrival > result.total_run_time) result.total_run_time = block->arrival;
		total_waiting_time += (result.total_run_time - block->arrival);

		result.total_run_time += block->remaining_burst_time;

		total_turnaround_time += (result.total_run_time - block->arrival);
	}
	result.average_waiting_time = (total_waiting_time / size);
	result.average_turnaround_time = (total_turnaround_time / size);

	return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result,
                 size_t quantum) {
  UNUSED(ready_queue);
  UNUSED(result);
  UNUSED(quantum);
  return false;
}

dyn_array_t *load_process_control_blocks(const char *input_file) {
  UNUSED(input_file);
  return NULL;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue,
                                   ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
}
