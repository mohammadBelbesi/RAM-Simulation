#ifndef EX5_SIM_MEM_H
#define EX5_SIM_MEM_H
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#define MEMORY_SIZE 16
extern char main_memory[MEMORY_SIZE];
typedef struct page_descriptor
{
    bool valid;
    bool dirty;
    int frame;
    int swap_index;
} page_descriptor;
class sim_mem {
    int swapfile_fd;
    int program_fd;
    int text_size;
    int data_size;
    int bss_size;
    int heap_stack_size;
    int num_of_pages;
    int page_size;
    page_descriptor **page_table; //pointer to page table
public:
    sim_mem(char exe_file_name[], char swap_file_name[], int
    text_size, int data_size, int bss_size, int heap_stack_size, int page_size);
    static void initializeMainMemory();
    void initializePageTable();
    void initializeSwapFile();
    void initializeFrameSpace() const;

    ~sim_mem();

    char load(int address);
    void store(int address, char value);
    char* readFromSwapFile(int page);
    int writeZerosToSwapFile(const char* zeros) const;
    char* readFromProgramFile(int page) const;
    char* allocateZerosPage() const;

    static void print_memory();
    void print_swap() const;
    void print_page_table();

    int writingToSwap(int startingFramePage);
    void cleanupAndExit();
    bool findEmptySpaceInSwap(int size, char* isZero, int& swapFreeSpace);
    char* copyMainMemoryToTempArray(int startingFramePage);
    void writeTempArrayToSwapFile(char* tmpArr, int swapFreeSpace);

    int allocatePageInMemory(int page, int offset, const char* pageCopy);
    int fillPageInMemory(int page, int offset, char* pageCopy);
    int findStartingFramePage();
    void resetPageTableEntries(int process_id, int startingFramePage);
    void copyPageToMainMemory(const char* pageCopy, int frame) const;

    int findFreeFrameIndex() const;

};
#endif
