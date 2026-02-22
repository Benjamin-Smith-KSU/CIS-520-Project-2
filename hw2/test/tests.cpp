#include "gtest/gtest.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

// Using a C library requires extern "C" to prevent function mangling
extern "C" {
#include <dyn_array.h>
#include <process_scheduling.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

TEST(fcfs, BadParams){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));

        dyn_array_t * queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(false, res); // capacity == 0
        dyn_array_destroy(queue);

        queue = dyn_array_create(NUM_PCB, 0, NULL);
        res = first_come_first_serve(queue, result);
        ASSERT_EQ(false, res); // data_size == 0
        dyn_array_destroy(queue);

        queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);
        res = first_come_first_serve(NULL, result);
        ASSERT_EQ(false, res);
        res = first_come_first_serve(queue, NULL);
        ASSERT_EQ(false, res);

        res = first_come_first_serve(queue, result);
        ASSERT_EQ(false, res); // count == 0

        dyn_array_destroy(queue);
        free(result);
}


TEST(fcfs, GoodParams){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        // given sorted
        for(size_t i = 0; i < 10; i++){
                ProcessControlBlock_t block;
                block.remaining_burst_time = (i < 1) ? 1 : i * 2;
                block.priority = 0;
                block.arrival = i;
                block.started = false;

                dyn_array_push_back(queue, &block);
        }
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(20.4f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(29.5f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(91UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);

        result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        // given unsorted
        for(size_t i = 0; i < 10; i+=2){ // even indicies
                ProcessControlBlock_t block;
                block.remaining_burst_time = (i < 1) ? 1 : i * 2;
                block.priority = 0;
                block.arrival = i;
                block.started = false;

                dyn_array_push_front(queue, &block);
        }
        for(size_t i = 1; i < 10; i+=2){ // odd indicies
                ProcessControlBlock_t block;
                block.remaining_burst_time = i * 2;
                block.priority = 0;
                block.arrival = i;
                block.started = false;

                dyn_array_push_front(queue, &block);
        }
        res = first_come_first_serve(queue, result);

        ASSERT_EQ(true, res);
        ASSERT_EQ(20.4f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(29.5f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(91UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);

}

TEST(fcfs, SinglePCB){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        ProcessControlBlock_t block;
        block.remaining_burst_time = 2;
        block.priority = 0;
        block.arrival = 1;
        block.started = false;

        dyn_array_push_front(queue, &block);
                
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(0.0f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(2.0f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(3UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);
}

TEST(fcfs, MultipleProcessesSameArrival){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        // given sorted
        for(size_t i = 0; i < 2; i++){
                ProcessControlBlock_t block;
                block.remaining_burst_time = 2;
                block.priority = 0;
                block.arrival = 1;
                block.started = false;

                dyn_array_push_back(queue, &block);
        }
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(1.0f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(3.0f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(5UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);
}

TEST(fcfs, CPUIdle){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        ProcessControlBlock_t block;
        block.remaining_burst_time = 2;
        block.priority = 0;
        block.arrival = 10;
        block.started = false;

        dyn_array_push_front(queue, &block);
                
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(0.0f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(2.0f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(12UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);
}

TEST(fcfs, ZeroBurstTime){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        ProcessControlBlock_t block;
        block.remaining_burst_time = 0;
        block.priority = 0;
        block.arrival = 1;
        block.started = false;

        dyn_array_push_front(queue, &block);
                
        bool res = first_come_first_serve(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(0.0f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(0.0f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(1UL, result ->total_run_time);// test total run time
        free(result);
        dyn_array_destroy(queue);
}


/*
unsigned int score;
unsigned int total;

class GradeEnvironment : public testing::Environment
{
  public:
    virtual void SetUp()
    {
      score = 0;
      total = 210;
    }

    virtual void TearDown()
    {
      ::testing::Test::RecordProperty("points_given", score);
      ::testing::Test::RecordProperty("points_total", total);
      std::cout << "SCORE: " << score << '/' << total <<
std::endl;
    }
};
*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  // ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
  return RUN_ALL_TESTS();
}

TEST(load_pcbs, AssignmentProvidedPCBS) {
  dyn_array_t *pcbs = load_process_control_blocks("../pcb.bin");

  ASSERT_EQ(4UL, dyn_array_size(pcbs));

  ProcessControlBlock_t pcb_0 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 0);
  ASSERT_EQ(15UL, pcb_0.remaining_burst_time);
  ASSERT_EQ(0UL, pcb_0.priority);
  ASSERT_EQ(0UL, pcb_0.arrival);
  ASSERT_EQ(0, pcb_0.started);

  ProcessControlBlock_t pcb_1 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 1);
  ASSERT_EQ(10UL, pcb_1.remaining_burst_time);
  ASSERT_EQ(0UL, pcb_1.priority);
  ASSERT_EQ(1UL, pcb_1.arrival);
  ASSERT_EQ(0, pcb_1.started);

  ProcessControlBlock_t pcb_2 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 2);
  ASSERT_EQ(5UL, pcb_2.remaining_burst_time);
  ASSERT_EQ(0UL, pcb_2.priority);
  ASSERT_EQ(2UL, pcb_2.arrival);
  ASSERT_EQ(0, pcb_2.started);

  ProcessControlBlock_t pcb_3 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 3);
  ASSERT_EQ(20UL, pcb_3.remaining_burst_time);
  ASSERT_EQ(0UL, pcb_3.priority);
  ASSERT_EQ(3UL, pcb_3.arrival);
  ASSERT_EQ(0, pcb_3.started);
}

TEST(load_pcbs, NonexistentFile) {
  dyn_array_t *pcbs = load_process_control_blocks("this-file-should-not-exist");
  ASSERT_EQ(NULL, pcbs);
}
