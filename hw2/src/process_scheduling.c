#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "dyn_array.h"
#include "process_scheduling.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block) {
  // decrement the burst time of the pcb
  --process_control_block->remaining_burst_time;
}

// static variable for fcfs compare
static dyn_array_t *static_queue = NULL;

// comapre function for fcfs, sorts indicies based on the value of that index in
// static_queue to avoid sorting the queue in place
int sortByArrivalTimeCompare(const void *x, const void *y) {
  size_t i = *(size_t *)x;
  size_t j = *(size_t *)y;
  ProcessControlBlock_t *block_x =
      (ProcessControlBlock_t *)dyn_array_at(static_queue, i);
  ProcessControlBlock_t *block_y =
      (ProcessControlBlock_t *)dyn_array_at(static_queue, j);
  int arrival_x = (int)block_x->arrival;
  int arrival_y = (int)block_y->arrival;
  int res = arrival_x - arrival_y;
  if (res == 0)
    return (i < j) ? -1 : 1;
  return res;
}

/// @brief This is a functions for Round Robin to compair arrival times for qsort
/// @param a PCB 1
/// @param b PCB 2
/// @return 1 if (a > b), -1 if (a < b) 0 if (a == b)
int comparePCBsByArrival(const void *a, const void *b) 
{
    ProcessControlBlock_t *pcb_a = (ProcessControlBlock_t *)a;
    ProcessControlBlock_t *pcb_b = (ProcessControlBlock_t *)b;
    
    if (pcb_a->arrival < pcb_b->arrival) return -1;
    if (pcb_a->arrival > pcb_b->arrival) return 1;
    return 0;
}

bool first_come_first_serve(dyn_array_t *ready_queue,
                            ScheduleResult_t *result) {
  if (ready_queue == NULL || result == NULL)
    return false;
  if (dyn_array_empty(ready_queue) || dyn_array_data_size(ready_queue) == 0 ||
      dyn_array_capacity(ready_queue) == 0)
    return false;

  float total_waiting_time = 0;
  float total_turnaround_time = 0;
  result->total_run_time = 0;
  const size_t size = dyn_array_size(ready_queue);

  size_t ind[size];
  for (size_t i = 0; i < size; i++) {
    ind[i] = i;
  }

  static_queue = ready_queue;
  qsort(ind, size, sizeof(size_t), sortByArrivalTimeCompare);
  static_queue = NULL;

  for (size_t i = 0; i < size; i++) {

    ProcessControlBlock_t *block = dyn_array_at(ready_queue, ind[i]);
    if (block == NULL)
      return false;

    if (block->arrival > result->total_run_time)
      result->total_run_time = block->arrival;
    total_waiting_time += (result->total_run_time - block->arrival);

    result->total_run_time += block->remaining_burst_time;

    total_turnaround_time += (result->total_run_time - block->arrival);
  }
  result->average_waiting_time = (total_waiting_time / size);
  result->average_turnaround_time = (total_turnaround_time / size);

  return true;
}

bool shortest_job_first(dyn_array_t *ready_queue,
                        ScheduleResult_t *result) { // non preemptive

  if (ready_queue == NULL || result == NULL)
    return false;
  if (dyn_array_empty(ready_queue) || dyn_array_data_size(ready_queue) == 0 ||
      dyn_array_capacity(ready_queue) == 0)
    return false;

  float total_waiting_time = 0;
  float total_turnaround_time = 0;
  result->total_run_time = 0;
  const size_t size = dyn_array_size(ready_queue);
  size_t jobs_ran = 0;

  size_t ind[size];
  for (size_t i = 0; i < size; i++) {
    ind[i] = i;
  }

  static_queue = ready_queue;
  qsort(ind, size, sizeof(size_t), sortByArrivalTimeCompare);
  static_queue = NULL;

  while (jobs_ran < size) {
    ssize_t lowest_burst_time_index = -1;
    uint32_t lowest_burst_time = 0xFFFFFFFF;

    for (size_t i = 0; i < size; i++) {
      ProcessControlBlock_t *block = dyn_array_at(ready_queue, ind[i]);
      if (block == NULL)
        return false;

      if (block->started == false && block->arrival <= result->total_run_time &&
          block->remaining_burst_time < lowest_burst_time) {
        lowest_burst_time_index = i;
        lowest_burst_time = block->remaining_burst_time;
      }
    }

    if (lowest_burst_time_index == -1) {
      uint32_t earliest_arrival = 0xFFFFFFFF;
      for (size_t i = 0; i < size; i++) {
        ProcessControlBlock_t *process = dyn_array_at(ready_queue, ind[i]);
        if (!process->started && process->arrival < earliest_arrival)
          earliest_arrival = process->arrival;
      }
      result->total_run_time = earliest_arrival;
      continue;
    }

    ProcessControlBlock_t *block =
        dyn_array_at(ready_queue, ind[lowest_burst_time_index]);

    total_waiting_time += (result->total_run_time - block->arrival);

    result->total_run_time += block->remaining_burst_time;

    total_turnaround_time += (result->total_run_time - block->arrival);

    block->started = true;
    jobs_ran++;
  }
  result->average_waiting_time = (total_waiting_time / size);
  result->average_turnaround_time = (total_turnaround_time / size);

  return true;
}

void pretty_print_pcb(ProcessControlBlock_t pcb) {
  printf("struct ProcessControlBlock_t {\n");
  printf("\tuint32_t remaining_burst_time: %d\n", pcb.remaining_burst_time);
  printf("\tuint32_t priority: %d\n", pcb.priority);
  printf("\tuint32_t arrival: %d\n", pcb.arrival);
  if (pcb.started) {
    printf("\tbool started: true\n");
  } else {
    printf("\tbool started: false\n");
  }
  printf("}\n");
}

// Compares two PCBs, first by earliest arrival time, but for equal arrival
// times we compare by highest priority (smallest integer ≡ highest priority)
//
// compare_priority(a, b) < 0 <=> a->arrival > b->arrival
//                                  or a->priority > b->priority
// compare_priority(a, b) = 0 <=> a->arrival = b->arrival
//                                  and a->priority = b->priority
// compare_priority(a, b) > 0 <=> a->arrival < b->arrival
//                                  or a->priority < b->priority
int compare_by_arrival_and_priority(const void *a, const void *b) {
  assert(a != NULL);
  assert(b != NULL);

  return ((ProcessControlBlock_t *)b)->priority -
         ((ProcessControlBlock_t *)a)->priority;
}

// The CPU is allocated to the process with the highest priority
// (smallest integer ≡ highest priority)
bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result) {
  if (NULL == ready_queue || NULL == result) {
    return false;
  }

  const size_t len_initial = dyn_array_size(ready_queue);
  if (0 == len_initial) {
    return false;
  }
  size_t len = len_initial;

  // Sort by arrival time and priority
  dyn_array_sort(ready_queue, compare_by_arrival_and_priority);

  // Store the initial value of remaining_burst_time for each PCB.
  // When a process finishes we will need it to compute their waiting time
  dyn_array_t *initial_burst_times =
      dyn_array_create(len_initial, sizeof(uint32_t), NULL);
  for (size_t i = 0; i < len_initial; i += 1) {
    ProcessControlBlock_t *pcb = dyn_array_at(ready_queue, i);
    dyn_array_push_back(initial_burst_times, &(pcb->remaining_burst_time));
  }

  // The total elapsed time since we began executing processes from the queue
  uint32_t total_run_time = 0;

  // The sum of time that each process takes to complete execution
  // starting from the time it arrived in the ready_queue
  uint32_t total_waiting_time = 0;

  // The sum of the amount of time that each process takes to complete execution
  // starting from the time it arrived in the ready_queue
  uint32_t total_turnaround_time = 0;

  while (len > 0) {
    // Start by assuming the highest priority PCB is just 0th index one
    ProcessControlBlock_t *pcb_highest_priority = dyn_array_at(ready_queue, 0);
    size_t index_highest_priority = 0;

    // If the 0th one hasn't arrived yet we have to "wait". We can begin
    // executing a process as soon the total_run_time is >= ->arrival.
    // The 0th index works because the PCBs have been sorted by arrival
    if (pcb_highest_priority->arrival > total_run_time) {
      total_run_time = pcb_highest_priority->arrival;
    }

    // Then check if any others have greater priority
    for (size_t i = 1; i < dyn_array_size(ready_queue); i += 1) {
      ProcessControlBlock_t *pcb = dyn_array_at(ready_queue, i);
      // PCB can't be null since we know
      // ready_queue != NULL and i < len
      assert(NULL != pcb);

      // If we've reached the end of those which have arrived, then since
      // they haven't arrived they can't be the highest priority
      if (pcb->arrival > total_run_time) {
        break;
      }

      // If this pcb has greater priority, set it as the new highest
      if (pcb->priority < pcb_highest_priority->priority) {
        pcb_highest_priority = pcb;
        index_highest_priority = i;
      }
    }

    assert(0 != pcb_highest_priority->remaining_burst_time);

    // We've successfully determined which PCB has the highest priority

    // Execute the process for 1 time slice
    pcb_highest_priority->remaining_burst_time -= 1;
    pcb_highest_priority->started = true;
    total_run_time += 1;

    // Compute metrics and remove this process if it has completed execution
    if (0 == pcb_highest_priority->remaining_burst_time) {
      // Turnaround time is the time elapsed between this process arriving in
      // the ready queue and now, when the process has completed execution
      u_int32_t turnaround_time =
          total_run_time - pcb_highest_priority->arrival;
      total_turnaround_time += turnaround_time;

      // The amount of time this process has been waiting in the ready queue
      // is equal to difference between its turnaround time
      // and its total burst time (the initial value of its burst time)
      total_waiting_time +=
          turnaround_time - *(uint32_t *)dyn_array_at(initial_burst_times,
                                                      index_highest_priority);

      dyn_array_erase(ready_queue, index_highest_priority);
      dyn_array_erase(initial_burst_times, index_highest_priority);
      len -= 1;
    }
  }

  result->average_waiting_time = (float)total_waiting_time / len_initial;
  result->average_turnaround_time = (float)total_turnaround_time / len_initial;
  result->total_run_time = total_run_time;

  return result;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) 
{
  //validate Parameters
  if(ready_queue == NULL || result == NULL || quantum == 0)
  {
    //Input parameter were not valid
    return false;
  } 

  //find how many processes we need to do
  const size_t total_processes = dyn_array_size(ready_queue);
  if (0 == total_processes) 
  {
    //invalid process size
    return false;
  }

  //sort by arival time 
  static_queue = ready_queue;
  qsort((void*)dyn_array_export(ready_queue), total_processes, sizeof(ProcessControlBlock_t), comparePCBsByArrival);
  static_queue = NULL;

  //create the work queue
  dyn_array_t* work_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t *), NULL);

  //save burst times to array
  uint32_t *burst_cache = (uint32_t*)malloc(sizeof(uint32_t) * total_processes);
  for(size_t i = 0; i < total_processes; i++)
  {
    ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, i);
    burst_cache[i] = pcb->remaining_burst_time;
  }

  //variables for tracking processes
  uint32_t current_time = 0;
  uint32_t total_waiting_time = 0;
  uint32_t total_turnaround_time = 0;
  size_t completed_processes = 0;
  size_t next_arrival_index = 0;

  //main loop
  while(completed_processes < total_processes)
  {
    //add any process that arrived at current time to the work queue
    while (next_arrival_index < total_processes) 
    {
      ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, next_arrival_index);
      if (pcb->arrival <= current_time) 
      {
          dyn_array_push_back(work_queue, &pcb);
          next_arrival_index++;
      } 
      else 
      {
        break;
      }
    }

    //if work queue is empty move to next arrival
    if(dyn_array_empty(work_queue))
    {
      current_time = ((ProcessControlBlock_t *)dyn_array_at(ready_queue, next_arrival_index))->arrival;
      continue;
    }

    //pop the next process from the front of the work queue
    ProcessControlBlock_t *current_pcb;
    dyn_array_extract_front(work_queue, &current_pcb);

    //find out how long to run the process
    uint32_t time_to_run;
    //if time remaining is less than quantum
    if(current_pcb->remaining_burst_time < quantum)
    {
      //finish out the process
      time_to_run = current_pcb->remaining_burst_time;
    }
    else
    {
      //else work on the process for quantum
      time_to_run = quantum;
    }

    //"Run" the process
    current_time += time_to_run;
    current_pcb->remaining_burst_time -= time_to_run;

    // Check for arrivals DURING the time slice before re-adding the current process
    while (next_arrival_index < total_processes) 
    {
      ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, next_arrival_index);
      if (pcb->arrival <= current_time) 
      {
        dyn_array_push_back(work_queue, &pcb);
        next_arrival_index++;
      } 
      else 
      {
        break;
      }
    }

    //re-add the current process if there is time left
    if(current_pcb->remaining_burst_time > 0)
    {
      dyn_array_push_back(work_queue, &current_pcb);
    }
    else
    {
      //process is finnished, now calc stats
      completed_processes++;
      uint32_t turnaround = current_time - current_pcb->arrival;
      total_turnaround_time += turnaround;

      //look through the burst_cache array to calc wait time
      for(size_t i=0; i < total_processes; i++) 
      {
        if (dyn_array_at(ready_queue, i) == current_pcb) 
        {
          total_waiting_time += (turnaround - burst_cache[i]);
          break;
        }
      }
    }


  }//end of main while loop
  
  //save results
  result->average_waiting_time = (float)total_waiting_time / total_processes;
  result->average_turnaround_time = (float)total_turnaround_time / total_processes;
  result->total_run_time = current_time;

  free(burst_cache);
  dyn_array_destroy(work_queue);
  return true;
}

dyn_array_t *load_process_control_blocks(const char *input_file) {
  // To quote the assignment description:
  // "The PCB file is composed of 1+(3*N) 32-bit integers. The first integer in
  // the file denotes the value of N. The following integers are organized into
  // triples: the first of each integer triple is the burst time of a process
  // in the ready queue; the second of each integer triple is the priority of
  // a process in the ready queue; the the third of each integer triple is the
  // time the process arrived in the ready queue."
  if (input_file == NULL) {
    return NULL;
  }
  const int fd = open(input_file, O_RDONLY);

  // Read the first integer N
  uint32_t n;
  const size_t size_of_n = sizeof(n);
  size_t bytes_read = read(fd, &n, size_of_n);
  if (size_of_n != bytes_read) {
    return NULL;
  }

  // Create an array for storing N PCB's
  dyn_array_t *pcbs = dyn_array_create(n, sizeof(ProcessControlBlock_t), NULL);

  // Declare a spare PCB so we have
  // something to read into before copying into the array
  ProcessControlBlock_t pcb_buf;
  // Since none of these processes have started, then
  // the .started field should be false for each newly loaded PCB
  pcb_buf.started = false;

  for (uint32_t i = 0; i < n; i += 1) {
    const size_t size_of_three_ints = 3 * sizeof(uint32_t);
    // Since the PCB struct has those three uint32_t's---in the same order as
    // our PCB file format---layed out in memory as its first 96 bits, then we
    // can just read those three uint32_t's at once directly into its first 96
    bytes_read = read(fd, (void *)(&pcb_buf), size_of_three_ints);

    if (size_of_three_ints != bytes_read) {
      dyn_array_destroy(pcbs);
      return NULL;
    }

    dyn_array_insert(pcbs, i, &pcb_buf);
  }

  // Check if we are actually at the end of the file
  // to ensure the file was formatted correctly and it wasn't just coincidence
  // that there were 1+(3*N) 32-bit integers in the file
  if (0 != read(fd, &n, sizeof(n))) {
    dyn_array_destroy(pcbs);
    return NULL;
  }

  return pcbs;
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue,
                                   ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
}
