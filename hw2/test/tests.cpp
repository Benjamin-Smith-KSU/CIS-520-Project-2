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

  ASSERT_EQ(4, dyn_array_size(pcbs));

  ProcessControlBlock_t pcb_0 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 0);
  ASSERT_EQ(15, pcb_0.remaining_burst_time);
  ASSERT_EQ(0, pcb_0.priority);
  ASSERT_EQ(0, pcb_0.arrival);
  ASSERT_EQ(0, pcb_0.started);

  ProcessControlBlock_t pcb_1 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 1);
  ASSERT_EQ(10, pcb_1.remaining_burst_time);
  ASSERT_EQ(0, pcb_1.priority);
  ASSERT_EQ(1, pcb_1.arrival);
  ASSERT_EQ(0, pcb_1.started);

  ProcessControlBlock_t pcb_2 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 2);
  ASSERT_EQ(5, pcb_2.remaining_burst_time);
  ASSERT_EQ(0, pcb_2.priority);
  ASSERT_EQ(2, pcb_2.arrival);
  ASSERT_EQ(0, pcb_2.started);

  ProcessControlBlock_t pcb_3 = *(ProcessControlBlock_t *)dyn_array_at(pcbs, 3);
  ASSERT_EQ(20, pcb_3.remaining_burst_time);
  ASSERT_EQ(0, pcb_3.priority);
  ASSERT_EQ(3, pcb_3.arrival);
  ASSERT_EQ(0, pcb_3.started);
}

TEST(load_pcbs, NonexistentFile) {
  dyn_array_t *pcbs = load_process_control_blocks("this-file-should-not-exist");
  ASSERT_EQ(NULL, pcbs);
}
