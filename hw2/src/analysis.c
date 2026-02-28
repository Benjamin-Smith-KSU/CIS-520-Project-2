#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "process_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRT "SRT"

/// @brief This program test the average wait, turnaround, and clock time
/// @param argc Number of arguments
/// @param argv The arguments
/// @return 
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
  //optional quantum
  size_t quantum = 0;

  //check if we have an optional quantum parm
  if (argc >= 4) 
  {
    //try to read quantum in as size_t
    if (sscanf(argv[3], "%zu", &quantum) != 1) 
    {
      //quantum read in failed
      fprintf(stderr, "Error: Invalid quantum value '%s'. Must be a positive integer.\n", argv[3]);
      return EXIT_FAILURE;
    }
  }

  //Load ready queue
  dyn_array_t *ready_queue = load_process_control_blocks(filename);

  //check if PCB loaded
  if(ready_queue == NULL)
  {
    //could not load pcb
    fprintf(stderr, "Error: Could not load PCB file %s\n", filename);
    return EXIT_FAILURE;
  }

  //create output stuct
  ScheduleResult_t result = {0, 0, 0};
  //create successes variable and set to false
  bool success = false;

  //identify algorithm and run it.
  if (strncmp(algorithm, FCFS, 5) == 0) //First Come First Serve
  {
    success = first_come_first_serve(ready_queue, &result);
  } 
  else if(strncmp(algorithm, SJF, 5) == 0)  //Shortest Job First
  {
    success = shortest_job_first(ready_queue, &result);
  }
  else if(strncmp(algorithm, P, 5) == 0)  //Priority
  {
    success = priority(ready_queue, &result);
  }
  else if(strncmp(algorithm, RR, 5) == 0)  //Round Robin
  {
    success = round_robin(ready_queue, &result, quantum);
  }
  else if(strncmp(algorithm, SRT, 5) == 0)  //Shortest Remaining Time first
  {
    success = shortest_remaining_time_first(ready_queue, &result);
  }
  else 
  {
    fprintf(stderr, "Error: Algorithm '%s' is not supported.\n", algorithm);
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
