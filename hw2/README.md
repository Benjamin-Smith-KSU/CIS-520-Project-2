# Project 2

Milestone 0: CMake and Unit Testing filled out

Milestone 1: PCB file loading and First Come First Served

Milestone 2: Shortest Job First, Shortest Remaining Time First, Round Robin, and analysis of algorithms

Note:
You can manually copy the time analysis from console and paste it to this file, but directly output from your program is strongly recommended.
---------------------------------------------------------------------------
Add your scheduling algorithm analysis below this line in a readable format.
---------------------------------------------------------------------------

$ ./analysis ../pcb.bin FCFS
Average Waiting Time:    16.00
Average Turnaround Time: 28.50
Total Clock Time:        50

$ ./analysis ../pcb.bin RR 4
Average Waiting Time:    24.00
Average Turnaround Time: 36.50
Total Clock Time:        50

$ ./analysis ../pcb.bin P
Average Waiting Time:    16.00
Average Turnaround Time: 28.50
Total Clock Time:        50

$ ./analysis ../pcb.bin SJF
Average Waiting Time:    14.75
Average Turnaround Time: 27.25
Total Clock Time:        50

$ ./analysis ../pcb.bin SRT
Average Waiting Time:    11.75
Average Turnaround Time: 24.25
Total Clock Time:        50