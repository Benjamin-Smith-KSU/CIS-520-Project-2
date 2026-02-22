#include "../include/processing_scheduling.h"
#include "gtest/gtest.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

// Using a C library requires extern "C" to prevent function mangling
extern "C" {
#include <dyn_array.h>
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
