#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "process_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRT "SRT"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv) 
{
  //check if the right number of arguments were pashed
  if (argc < 3) 
  {
    printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
    return EXIT_FAILURE;
  }

  //read in the file name
  char *filename = argv[1];
  //read in the algorithm 
  char *algorithm = argv[2];


  //Load ready queue
  dyn_array_t *ready_queue = load_process_control_blocks(filename);
  //check if memory loaded
  if(ready_queue == NULL)
  {
    fprintf(stderr, "Error: Could not load PCB file %s\n", filename);
    return EXIT_FAILURE;
  }

  //create output stuct
  ScheduleResult_t result = {0, 0, 0};
  bool success = false;

  //identify algorithm and run it.
  if (strncmp(algorithm, FCFS, 5) == 0) 
  {
    success = first_come_first_serve(ready_queue, &result);
  } 
  else 
  {
    fprintf(stderr, "Error: Algorithm '%s' is not supported in Checkpoint 2.\n", algorithm);
  }

  //print the results
  if (success) 
  {
      printf("Average Waiting Time:    %.2f\n", result.average_waiting_time);
      printf("Average Turnaround Time: %.2f\n", result.average_turnaround_time);
      printf("Total Clock Time:        %lu\n", result.total_run_time);
  }

  //clean up alocated memory
  dyn_array_destroy(ready_queue);
  return EXIT_SUCCESS;
}
