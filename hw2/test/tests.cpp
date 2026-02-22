#include "../include/processing_scheduling.h"
#include "gtest/gtest.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>

// Using a C library requires extern "C" to prevent function mangling
extern "C" {
#include <dyn_array.h>
#include <processing_scheduling.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

TEST(first_come_first_serve, BadParams){
        ScheduleResult_t * result = malloc(sizeof(ScheduleResult_t));

        dyn_array_t * queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
        ASSERT_EQ(false, first_come_first_serve(queue, result)); // capacity == 0
        free(queue);

        queue = dyn_array_create(NUM_PCB, 0, NULL);
        ASSERT_EQ(false, first_come_first_serve(queue, result)); // data_size == 0
        free(queue);

        queue = dyn_array_create(NUM_PCB, sizeof(ProcessControlBlock_t), NULL);
        ASSERT_EQ(false, first_come_first_serve(NULL, result));
        ASSERT_EQ(false, first_come_first_serve(queue, NULL));

        ASSERT_EQ(false, first_come_first_serve(queue, result)); // count == 0

        free(queue);
        free(result);
}

TEST(first_come_first_serve, GoodParams){

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
