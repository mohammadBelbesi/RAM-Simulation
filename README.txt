Virtual Memory
program name: Virtual Memory
Mohammad Belbesi



===Description Virtual Memory ===
The program (code) is deisgned to act like virtual memory with load and store founction
*the program will define every variable there is.
*it will fill the page table with the stander values.
*it will fill the main memory with zeros.
*it will creat the execablue file , and if it's not there it will print false sentence.
*it will creat the swap file.
*it will start working so if we started filling the page table and the main memory using the Store function it will start choose the rigth frame and page for each input , and put every thing in the right place.
*the main function is the memory check that checks if there is an empty place in the main memory or not , and if there is no it will make a place for it , with the help of the swap file.
*after filling the main memory with some text , data , bss , etc we will be able to call it using the function load from the main memory.


==Program Files==
main.cpp
sim_mem.cpp
sim_mem.h

==functions==
three main functions:
1-load
2-store
3-main

other main functions:

1-constructor
2-destructor
3-print_memory
4-print_swap
5-print_page_table

my side functions:
  
initializeMainMemory()
initializePageTable()
initializeSwapFile()
initializeFrameSpace()
readFromSwapFile(int page)
writeZerosToSwapFile(const char* zeros)
readFromProgramFile(int page)
allocateZerosPage()
writingToSwap(int startingFramePage)
cleanupAndExit()
findEmptySpaceInSwap(int size, char* isZero, int& swapFreeSpace)
copyMainMemoryToTempArray(int startingFramePage)
writeTempArrayToSwapFile(char* tmpArr, int swapFreeSpace)
allocatePageInMemory(int page, int offset, const char* pageCopy)
fillPageInMemory(int page, int offset, char* pageCopy)
findStartingFramePage()
resetPageTableEntries(int process_id, int startingFramePage)
copyPageToMainMemory(const char* pageCopy, int frame)
findFreeFrameIndex()

==How to compile and run?==
you can use the Clion run button :)
make sure the cmake is like this : 
cmake_minimum_required(VERSION 3.25)
project(Virtual Memory)

set(CMAKE_CXX_STANDARD 14)

add_executable(VirtualMemory sim_mem.h sim_mem.cpp main.cpp)
**********************************************
in Clion terminal:
g++ main.cpp sim_mem.h sim_mem.cpp -o main
run: ./main
**********************************************
in linux terminal:
make sure you are in the right folder and then :
g++ main.cpp sim_mem.h sim_mem.cpp -o VirtualMemory
run: ./VirtualMemory
**********************************************
you can also go in to the folder and open terminal and the :
type : make
then type: ./app

====InPut====
in the main function type the follow code:
    /////////////////////////////////////////////////////////////////////////////
    sim_mem s((char*)"exec_file1", (char*)"swap_file" ,25, 50, 25,25, 25, 5);
    //sim_mem s((char*)"exec_file2", (char*)"swap_file" ,16, 16, 32,32, 16, 8);

    s.load (1);
    s.store( 26,'y');
    s.store( 59,'x');
    s.store( 31,'y');
    s.store( 65,'x');

    s.print_memory();
    s.print_swap();
    s.print_page_table();
    /////////////////////////////////////////////////////////////////////////////

====OutPut====
Physical memory
[0] [a]
[1] [y]
[2] [a]
[3] [a]
[4] [a]
[5] [b]
[6] [b]
[7] [b]
[8] [b]
[9] [x]
[10] [b]
[11] [y]
[12] [b]
[13] [b]
[14] [b]
[15] [x]
[16] [d]
[17] [d]
[18] [d]
[19] [d]
[20] [0]
[21] [0]
[22] [0]
[23] [0]
[24] [0]
[25] [0]
........
......
....
..
[197] [0]
[198] [0]
[199] [0]

Swap memory
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	
0 - [0]	1 - [0]	2 - [0]	3 - [0]	4 - [0]	

Valid	 Dirty	 Frame	 Swap index
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]

Valid	 Dirty	 Frame	 Swap index
[1]	[1]	[0]	[-1]
[1]	[1]	[2]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[1]	[1]	[1]	[-1]
[0]	[0]	[-1]	[-1]
[1]	[1]	[3]	[-1]
[0]	[0]	[-1]	[-1]

Valid	 Dirty	 Frame	 Swap index
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]

Valid	 Dirty	 Frame	 Swap index
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]
[0]	[0]	[-1]	[-1]


