#include <fcntl.h>
#include <stdio.h>
#include <string.h>
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

bool first_come_first_serve(dyn_array_t *ready_queue,
                            ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
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

void pretty_print_pcb(ProcessControlBlock_t pcb) {
  printf("struct ProcessControlBlock_t {\n");
  printf("\tuint32_t remaining_burst_time: %d\n", pcb.remaining_burst_time);
  printf("\tuint32_t priority: %d\n", pcb.priority);
  printf("\tuint32_t arrival: %d\n", pcb.arrival);
  printf("\tbool started: %b\n", pcb.started);
  printf("}\n");
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue,
                                   ScheduleResult_t *result) {
  UNUSED(ready_queue);
  UNUSED(result);
  return false;
}
