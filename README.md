# CIS-520-Project-2
This is the github repo for group 16 for KSU CIS 520
Project 2: Process Scheduling
This is a group project. You can view your group assignment under the "People" page on Canvas (https://k-state.instructure.com/courses/187992/groups). You are responsible for getting in touch with your group members to start collaborating. Don't put this off!!
(You can see a sample of the group evaluation survey here)

1. Introduction
This project will expose you to the simplicity and beauty of CPU scheduling algorithms. You are to implement scheduling algorithms to process tasks from the ready queue on a virtual CPU. You will also collect statistics of the various times (wait time, turnaround time, etc.).

2. Objectives:
Understand the creation and use of a CMake project
Implement unit tests for Test-Driven Development (TDD)
Implement process scheduling algorithms
Understand the performance trade-offs for various scheduling algorithms
3. Requirements:
Implement First Come First Served (FCFS), Shortest Job First (SJF), Round Robin (RR), Shortest Remaining Time First (SRTF), and priority scheduling algorithms. Your analysis program (driver) must read the binary data file and run its tasks on the virtual CPU using the appropriate algorithm based on the command line arguments.

Notice that multiple scheduling algorithms may prototyped in the include/processing_scheduling.h header file as well as "stubbed" in the src/processing_scheduling.c implementation file (to facilitate changes from semester to semester). You will only need to implement the required ones, below. This semester, you must implement all of them.

4. Directions:
As in the previous project, you will need to login to Beocat and copy the project files to your home directory. After you have connected to Beocat, copy the project files to your home directory. Project files are in the "Files / Projects" directory. The ones for Homework 2 are hw2.zip Download hw2.zip.

Next, you need to extract the contents of the zipped file. For this, you need to enter the following command:

unzip hw2.zip

All required files should now be extracted to the folder "hw2". Before you can start working on the project, however, you will need to load the required modules (cmake, googletest, and foss). To do so, enter the following command:

module load googletest/1.11.0-GCCcore-11.3.0 CMake/3.21.1-GCCcore-11.2.0 foss/2021b

Complete information on the available modules (and how to load and unload them) can be found on this pageLinks to an external site..

You may now start programming. Refer to the project description file for the detailed description of the project.

5. Details:
Input Data, Process Scheduling:
A data file will be provided for your program. The binary file format is provided further below. Your schedulers will need to track metadata such as average wait time, average turnaround time, and total run time.

Binary Configuration File Format:

number of process control blocks (32-bit unsigned)
first process burst time
first process priority
first process arrival time
second process burst time
second process priority
second process arrival time
…
Nth process burst time
Nth process priority
Nth process arrival time
The PCB file is composed of 1+(3*N) 32-bit integers. The first integer in the file denotes the value of N. The following integers are organized into triples: the first of each integer triple is the burst time of a process in the ready queue; the second of each integer triple is the priority of a process in the ready queue; the the third of each integer triple is the time the process arrived in the ready queue. Priority will be represented by a positive integer, with a smaller number meaning a higher priority.

Terminology and Concepts:

burst time: the runtime of the process through our virtual CPU
arrival time: the time the process arrived into our ready queue
wait time: the time between arrival of the process and the first time the process is scheduled to run on the CPU (including time waiting in the ready queue) plus any additional time the process could run on the CPU but is instead in the ready queue
turnaround time: the time a process takes to complete (from arrival to completion)
Resources:

Please refer to posted slides and the textbook
The Linux/Unix programmer's manual, accessible through the "man pages" of the Beocat environment
Project_2_Session.pdf
Project_2_SessionA.pdf
In each function below dyn_array_t* ready_queue is a linear structure holding a collection of Process Control Blocks. If you need guidance on the usage of dyn_array, review the documentation in the include/dyn_array.h header file.

To get started, you will review the provided code, including header files for your function implementations that need to be unit tested. You must implement the CMakeLists.txt file based on the provided comments. You will add at least 10 distinct and relevant unit tests to the tester (test/tests.cpp). You should use these tests to help motivate and validate your implementation as you move forward with the project. You must implement at least 2 tests per function in the include/processing_scheduling.h header file.

CMake resources:
Your best bet is to build off the examples you had in Project 1. Nevertheless, here are some additional resources "curated" for you.

LLVM's CMake PrimerLinks to an external site.
Onur Dündar's CMake TutorialLinks to an external site.
Daniel Pfeifer's Effective CMake (video)Links to an external site.
Official CMake TutorialLinks to an external site.
You may want to start here: "Step 1: A Basic Starting PointLinks to an external site."
Note that we may provide test cases in future milestones. Our test cases may supersede your tests, or your improved understanding of implementation may lead you to remove some of your tests. As such, it is not required that you maintain (continue to use) your original tests in future milestones. Nonetheless, good test cases can help you design and develop better code so you are encouraged to write good tests in this milestone.

PCB loading function prototype:

dyn_array_t* load_process_control_blocks (const char* input_file);
Scheduling algorithm function prototypes:

bool first_come_first_serve (dyn_array_t* ready_queue, ScheduleResult_t* result);
bool shortest_job_first (dyn_array_t* ready_queue, ScheduleResult_t* result);
bool priority (dyn_array_t *ready_queue, ScheduleResult_t* result);
bool round_robin (dyn_array_t* ready_queue, ScheduleResult_t* result, size_t quantum);
bool shortest_remaining_time_first (dyn_array_t* ready_queue, ScheduleResult_t* result);
Note that the main difference between shortest_job_first and shortest_remaining_time_first is that the latter function is preemptive.
priority is NOT preemptive.

Analysis:

Your analysis.c program should compute the following statistics for each scheduling algorithm:

float Average Waiting Time
float Average Turnaround Time
unsigned long Total Clock Time
This information should be recorded in a human-readable format in the README.md file.

NOTE: For the above process scheduling algorithms use your book, online resources, and class notes for implementation. You do not need to implement any low-level context switching, the real Linux ready queue, or CPU driver.

To allow your analysis.c to be built, you must update the CMakeLists.txt. The program name must be analysis Your program will need to read the necessary parameters from the command line as shown below:

analysis <PCBs_bin_file> <schedule algorithm> [Optional_Time_Quantum]
The number of schedule parameters depends on the scheduling algorithm. Please review this document and the provided source files in hw2.zip Download hw2.zipfor additional details!

The flow of the analysis program is:

Load process control blocks from binary file passed at the command line into a dyn_array (this is your ready queue).
Execute your scheduling algorithm to collect the statistics
Clean up any allocations
Report your times to stdout using either printf or fprintf (copy these to the README.md file)
IMPORTANT TESTING NOTE:

We will run tests by constructing our own dyn_array_t structures. DO NOT ASSUME that a structure passed into one of the functions above was constructed by you. This also implies that you cannot modify the dyn_array_t type because then we won't know how to construct one that your code "understands".

Additional Process Scheduling Notes:

The clock time starts at zero and increments until the ready queue is empty.

Lastly, each scheduling function fills out a Schedule Result structure that contains average turnaround time, and average wait time, and total clock time used for verification and analysis.

You are required to run a test and report the timings in your README.md for each scheduling algorithm.

Your analysis program should be run with the time command as follows, example:

time ./analysis pcb.bin RR 4
Rubric (50 points possible):
2 pts for correct CMakeLists.txt creation
1 pts for each reasonable and appropriate test, up to 2 per function in the header ("shortest job first", "first come, first served", "round robin", "shortest remaining time first", and "load PCB" functions). Note that you may change your tests in future milestones (because you may not fully understand things at first), but you should strive to have good tests from the beginning if possible.

13 pts points for passing OUR testing system (e.g, Build Tests, Unit tests, Valgrind, including tests for memory leaks)
5 pts for correct PCB loading
5 pts for correct implementation of FCFS
5 pts for correct implementation of the priority function

3 pts for correct implementation of Shortest Job First scheduling
3 pts for correct implementation of Round Robin scheduling
3 pts for scheduling runtime analysis
3 pts for correct implementation of Shortest Remaining Time First scheduling

Insufficient Parameter Validation: Deduction of up to 20% of rubric score
Insufficient Error Checking: Deduction of up to 20% of rubric score
Insufficient Block and Inline Comments: Deduction of up to 20% of rubric score
Peer evaluation: Deduction of up to 20% of rubric score
