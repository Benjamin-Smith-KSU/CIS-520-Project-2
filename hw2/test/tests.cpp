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

TEST(sjf, BadParams){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));

        dyn_array_t * queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
        bool res = shortest_job_first(queue, result);
        ASSERT_EQ(false, res); // capacity == 0
        dyn_array_destroy(queue);

        queue = dyn_array_create(NUM_PCB, 0, NULL);
        res = shortest_job_first(queue, result);
        ASSERT_EQ(false, res); // data_size == 0
        dyn_array_destroy(queue);

        queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);
        res = shortest_job_first(NULL, result);
        ASSERT_EQ(false, res);
        res = shortest_job_first(queue, NULL);
        ASSERT_EQ(false, res);

        res = shortest_job_first(queue, result);
        ASSERT_EQ(false, res); // count == 0

        dyn_array_destroy(queue);
        free(result);
}

TEST(sjf, GoodParams){
        ScheduleResult_t * result = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
        dyn_array_t * queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);

        uint32_t bursts[10]  = {20, 1, 1, 2, 2, 3, 3, 4, 4, 5};
        uint32_t arrivals[10]= {0,  1, 2, 3, 4, 5, 6, 7, 8, 9};

        for(size_t i = 0; i < 10; i++){
                ProcessControlBlock_t block;
                block.remaining_burst_time = bursts[i];
                block.priority = 0;
                block.arrival = arrivals[i];
                block.started = false;

                dyn_array_push_back(queue, &block);
        }
        bool res = shortest_job_first(queue, result);
        ASSERT_EQ(true, res);
        ASSERT_EQ(20.5f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(25.0f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(45.0f, result ->total_run_time);// test total run time
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
        res = shortest_job_first(queue, result);

        ASSERT_EQ(true, res);
        ASSERT_EQ(20.4f, result->average_waiting_time);// test avg waiting time
        ASSERT_EQ(29.5f, result->average_turnaround_time);// test avg turnaround time
        ASSERT_EQ(91UL, result ->total_run_time);// test total run time
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

TEST(priority, OnePCB) {
  dyn_array_t *ready_queue = dyn_array_create(2, sizeof(ProcessControlBlock_t), NULL);
  ScheduleResult_t result;
  ProcessControlBlock_t pcb;

  pcb.remaining_burst_time = 3;
  pcb.priority = 0;
  pcb.arrival = 2;
  pcb.started = false;
  dyn_array_insert(ready_queue, 0, &pcb);

  priority(ready_queue, &result);

  ASSERT_EQ(0.f, result.average_waiting_time);
  ASSERT_EQ(3.f, result.average_turnaround_time);
  ASSERT_EQ(5UL, result.total_run_time);
}

// This test test the scenario where a higher priority process arrives later
// than a lower priority process, and the scheduler needs to change which
// process is currently being executed.
TEST(priority, TwoPCBsSwitchingExecution) {
  dyn_array_t *ready_queue = dyn_array_create(2, sizeof(ProcessControlBlock_t), NULL);
  ScheduleResult_t result;
  ProcessControlBlock_t pcb;

  // This process, A, has higher priority but it arrives after the process B
  pcb.remaining_burst_time = 3;
  pcb.priority = 0;
  pcb.arrival = 2;
  pcb.started = false;
  dyn_array_insert(ready_queue, 0, &pcb);

  // This process, B, has lower priority but it arrives before the process A
  pcb.remaining_burst_time = 2;
  pcb.priority = 1;
  pcb.arrival = 1;
  pcb.started = false;
  dyn_array_insert(ready_queue, 0, &pcb);

  priority(ready_queue, &result);

  // The line below shows which process is being execute during each time slice.
  // '_' is used to show that neither A nor B is executing during time = 0.
  // _BAAAB

  // A has a waiting time of 3-3 = 0
  // B has a waiting time of 5-2 = 3
  // A has a turnaround time of 3-0 = 3
  // B has a turnaround time of 6-1 = 5

  ASSERT_EQ((0.f + 3.f) / 2.f, result.average_waiting_time);
  ASSERT_EQ((3.f + 5.f) / 2.f, result.average_turnaround_time);
  ASSERT_EQ(6UL, result.total_run_time);
}



TEST(RoundRobin, BadParam_NullPointers) 
{
    ScheduleResult_t res;
    // Test 1: NULL ready_queue
    // The function should detect the NULL and return false immediately.
    EXPECT_FALSE(round_robin(NULL, &res, 4));

    // Test 2: NULL result struct
    dyn_array_t* dq = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(round_robin(dq, NULL, 4));
    
    dyn_array_destroy(dq);
}

TEST(RoundRobin, BadParam_ZeroQuantum) {
    dyn_array_t* dq = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    ProcessControlBlock_t p1 = {5, 1, 0, false};
    dyn_array_push_back(dq, &p1);

    ScheduleResult_t res;
    // A quantum of 0 would cause an infinite loop or division by zero.
    // Your code should catch this and return false.
    EXPECT_FALSE(round_robin(dq, &res, 0));

    dyn_array_destroy(dq);
}

TEST(RoundRobin, GoodParam_SingleProcess) {
    dyn_array_t* dq = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    
    // Process: Burst 10, Arrival 0
    ProcessControlBlock_t p1 = {10, 1, 0, false};
    dyn_array_push_back(dq, &p1);

    ScheduleResult_t res;
    size_t quantum = 2; // Quantum doesn't matter for 1 process, it will just cycle.

    bool success = round_robin(dq, &res, quantum);

    ASSERT_TRUE(success);
    // For a single process:
    // Wait time must be 0 (it arrived and started immediately).
    EXPECT_FLOAT_EQ(res.average_waiting_time, 0.0f);
    // Turnaround time must equal burst time (10 - 0 = 10).
    EXPECT_FLOAT_EQ(res.average_turnaround_time, 10.0f);
    // Total clock time must be 10.
    EXPECT_EQ(res.total_run_time, (uint32_t)10);

    dyn_array_destroy(dq);
}

TEST(RoundRobin, BasicPreemption) 
{
    dyn_array_t* dq = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    
    // Process 1: Burst 6, Arrival 0
    ProcessControlBlock_t p1 = {6, 0, 0, false}; 
    // Process 2: Burst 2, Arrival 0
    ProcessControlBlock_t p2 = {2, 0, 0, false};
    
    dyn_array_push_back(dq, &p1);
    dyn_array_push_back(dq, &p2);

    ScheduleResult_t res;
    size_t quantum = 4;
    
    // Logic: 
    // Time 0-4: P1 runs (2 left). 
    // Time 4-6: P2 runs (0 left) -> P2 Finishes.
    // Time 6-8: P1 runs (0 left) -> P1 Finishes.
    
    bool success = round_robin(dq, &res, quantum);

    ASSERT_TRUE(success);
    EXPECT_EQ(res.total_run_time, (uint32_t)8);
    // P2 waited 4 units. P1 waited 2 units (the time P2 was running).
    // Total wait = 6. Avg = 3.0
    EXPECT_FLOAT_EQ(res.average_waiting_time, 3.0f);
    // P2 turnaround = 6. P1 turnaround = 8.
    // Total TA = 14. Avg = 7.0
    EXPECT_FLOAT_EQ(res.average_turnaround_time, 7.0f);
    
    dyn_array_destroy(dq);
}

TEST(RoundRobin, StaggeredArrival) 
{
    dyn_array_t* dq = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    
    // Process 1: Burst 5, Arrival 0
    ProcessControlBlock_t p1 = {5, 0, 0, false}; 
    // Process 2: Burst 3, Arrival 2 (Arrives while P1 is running)
    ProcessControlBlock_t p2 = {3, 0, 2, false};
    
    dyn_array_push_back(dq, &p1);
    dyn_array_push_back(dq, &p2);

    ScheduleResult_t res;
    size_t quantum = 3;
    
    // Timeline:
    // 0-3: P1 runs (2 left). P2 arrives at T=2.
    // 3-6: P2 runs (0 left). P2 finishes.
    // 6-8: P1 runs (2 left). P1 finishes.
    
    bool success = round_robin(dq, &res, quantum);

    ASSERT_TRUE(success);
    EXPECT_EQ(res.total_run_time, (uint32_t)8);
    
    // P1 wait: spent 3 units in queue while P2 ran.
    // P2 wait: arrived at 2, started at 3. (1 unit).
    // Avg Wait: (3 + 1) / 2 = 2.0
    EXPECT_FLOAT_EQ(res.average_waiting_time, 2.0f);
    
    dyn_array_destroy(dq);
}
