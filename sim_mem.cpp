#include "sim_mem.h"
int * frameSpace;
int nextToSwap;

sim_mem::sim_mem(char exe_file_name[], char swap_file_name[], int text_size, int data_size,
                 int bss_size, int heap_stack_size, int page_size) {
    if (exe_file_name == nullptr) {
        perror("exe_file_name is NULL");
        exit(EXIT_FAILURE);
    }

    // Open the executable file
    program_fd = open(exe_file_name, O_RDONLY, 0);
    if (program_fd == -1) {
        std::cout << "Failed to open executable file." << std::endl;
        perror("open() failed");
        exit(EXIT_FAILURE);
    }

    // Open or create the swap file
    this->swapfile_fd = open(swap_file_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (this->swapfile_fd == -1) {
        perror("Failed to open swap file.");
        close(program_fd);
        exit(EXIT_FAILURE);
    }

    // Initialize memory sizes and settings
    this->text_size = text_size;
    this->data_size = data_size;
    this->bss_size = bss_size;
    this->heap_stack_size = heap_stack_size;
    this->page_size = page_size;
    nextToSwap = 0;

    // Initialize main memory
    initializeMainMemory();

    // Initialize page table
    initializePageTable();

    // Initialize the swap file
    initializeSwapFile();

    // Initialize the frame space
    initializeFrameSpace();
}

void sim_mem::initializeMainMemory() {
    // Fill main memory with zeros
    std::fill(std::begin(main_memory), std::end(main_memory), '0');
}
void sim_mem::initializePageTable() {
    // Create page table array
    page_table = new page_descriptor *[1];
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    // Create page descriptors for each page in the table and initialize with default values
    page_table[0] = new page_descriptor[pagesNum];
    std::fill_n(page_table[0], pagesNum, page_descriptor{false, false, -1, -1});
}

void sim_mem::initializeSwapFile() {
    // Calculate the size of the swap file
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    int size = (this->page_size * (pagesNum - (this->text_size / this->page_size)));

    // Create an array to initialize the swap file with zeros
    char *init = new char[size + 1];
    std::fill_n(init, size, '0');
    init[size] = '\0';

    // Write the initialization array to the swap file
    if (write(this->swapfile_fd, init, size) == -1) {
        perror("Failed to write to swap file");
        delete[] page_table[0];
        delete[] page_table;
        close(program_fd);
        close(swapfile_fd);
        delete[] init;
        exit(EXIT_FAILURE);
    }

    delete[] init;
}

void sim_mem::initializeFrameSpace() const {
    // Create the frameSpace array and fill it with zeros
    frameSpace = new int[MEMORY_SIZE / this->page_size];
    std::fill_n(frameSpace, MEMORY_SIZE / this->page_size, 0);
}

int sim_mem::findFreeFrameIndex() const {
    int i;
    // Iterate over the frameSpace array to find the index of the first free frame
    for (i = 0; i < MEMORY_SIZE/ this->page_size; i ++) {
        if (frameSpace[i] == 0)
            break;
    }
    return i ;
}

int sim_mem::writingToSwap(int startingFramePage) {
    // Calculate the size of the remaining space in the swap file
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    int size = this->page_size * (pagesNum - (this->text_size / this->page_size));

    // Variables for checking empty space in the swap file
    char isZero[1];
    int swapFreeSpace;

    // Set the file pointer to the beginning of the swap file
    if (lseek(swapfile_fd, 0, SEEK_SET) == -1) {
        perror("lseek() failed");
        cleanupAndExit();
    }

    // Find an empty space in the swap file
    bool foundEmpty = findEmptySpaceInSwap(size, isZero, swapFreeSpace);

    if (!foundEmpty) {
        std::cerr << "Error: No empty space found in the swapfile." << std::endl;
        cleanupAndExit();
    }

    // Copy the contents of the main memory to a temporary array
    char* tmpArr = copyMainMemoryToTempArray(startingFramePage);

    // Write the temporary array to the swap file
    writeTempArrayToSwapFile(tmpArr, swapFreeSpace);

    return swapFreeSpace;
}

void sim_mem::cleanupAndExit() {
    // Clean up resources and exit the program
    delete page_table[0];
    delete[] page_table;
    delete[] frameSpace;
    close(program_fd);
    close(swapfile_fd);
    exit(EXIT_FAILURE);
}
bool sim_mem::findEmptySpaceInSwap(int size, char* isZero, int& swapFreeSpace) {
    int j = 0;
    while (j < size) {
        // Check if the current space in the swap file is empty
        bool isEmpty = true;
        int i = j;
        while (i < j + page_size) {
            // Read a single character from the swap file
            if (read(swapfile_fd, isZero, 1) == -1) {
                perror("read() failed");
                cleanupAndExit();
            }
            // If the character is not '0', the space is not empty
            if (isZero[0] != '0') {
                isEmpty = false;
                break;
            }
            i++;
        }

        // If an empty space is found, update the swapFreeSpace variable and return true
        if (isEmpty) {
            swapFreeSpace = j;
            return true;
        }

        j += page_size;
    }

    return false;
}

char* sim_mem::copyMainMemoryToTempArray(int startingFramePage) {
    // Allocate memory for the temporary array
    char* tmpArr = (char*)malloc(sizeof(char) * page_size);
    int mainIndex = (page_table[0][startingFramePage].frame) * page_size;
    int j = 0;
    while (j < page_size) {
        // Copy contents from the main memory to the temporary array
        tmpArr[j] = main_memory[mainIndex + j];
        j++;
    }

    return tmpArr;
}

void sim_mem::writeTempArrayToSwapFile(char* tmpArr, int swapFreeSpace) {
    // Set the file pointer to the swapFreeSpace position in the swap file
    if (lseek(swapfile_fd, swapFreeSpace, SEEK_SET) == -1) {
        perror("lseek() failed");
        cleanupAndExit();
    }

    // Write the contents of the temporary array to the swap file
    if (write(swapfile_fd, tmpArr, page_size) == -1) {
        perror("write() failed");
        cleanupAndExit();
    }
}

int sim_mem::allocatePageInMemory(int page, int offset, const char* pageCopy) {
    // Reset the nextToSwap index if it reaches the end of the main memory
    if (nextToSwap == MEMORY_SIZE / page_size) {
        nextToSwap = 0;
    }

    int new_process_id = 0;
    int startingFramePage = findStartingFramePage();

    // If the previous page in the starting frame was modified, write it to the swap file
    if (page_table[new_process_id][startingFramePage].dirty) {
        page_table[new_process_id][startingFramePage].swap_index = writingToSwap(startingFramePage);
    }

    // Reset page table entries and copy the new page to the main memory
    resetPageTableEntries(new_process_id, startingFramePage);
    page_table[0][page].frame = nextToSwap;
    copyPageToMainMemory(pageCopy, nextToSwap);

    nextToSwap++;
    page_table[0][page].valid = true;
    frameSpace[page_table[0][page].frame] = 1;

    return ((page_table[0][page].frame * page_size) + offset);
}

int sim_mem::fillPageInMemory(int page, int offset, char* pageCopy) {
    // Find a free frame in the main memory
    int tmpFrame = findFreeFrameIndex();

    // If no free frame is available, allocate a new page in the main memory
    if (tmpFrame == MEMORY_SIZE / page_size) {
        return allocatePageInMemory(page, offset, pageCopy);
    }

    // Assign the page to the free frame and copy the page contents to the main memory
    page_table[0][page].frame = tmpFrame;
    copyPageToMainMemory(pageCopy, tmpFrame);

    page_table[0][page].valid = true;
    frameSpace[page_table[0][page].frame] = 1;

    free (pageCopy);

    return ((page_table[0][page].frame * page_size) + offset);
}

int sim_mem::findStartingFramePage() {
    int new_process_id = 0;
    int startingFramePage = 0;
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    // Find the starting frame page in the page table
    while (startingFramePage < pagesNum) {
        if (page_table[new_process_id][startingFramePage].frame == nextToSwap) {
            break;
        }
        startingFramePage++;
    }

    return startingFramePage;
}

void sim_mem::resetPageTableEntries(int process_id, int startingFramePage) {
    // Reset the frame and validity of the page table entry
    page_table[process_id][startingFramePage].frame = -1;
    page_table[process_id][startingFramePage].valid = false;
}

void sim_mem::copyPageToMainMemory(const char* pageCopy, int frame) const {
    int j = 0;
    int i = frame * page_size;
    while (i < (frame * page_size) + page_size) {
        // Copy the page contents to the main memory
        main_memory[i] = pageCopy[j];
        j++;
        i++;
    }
}

char sim_mem::load(int address) {
    // Check if the address is valid
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    if (address >= page_size * pagesNum) {
        perror("Invalid address");
        cleanupAndExit();
    }

    char* tmpArr;
    int page = address / page_size;
    int offset = address % page_size;

    // Check if the page is already in main memory
    if (page_table[0][page].valid) {
        return main_memory[(page_table[0][page].frame * page_size) + offset];
    }

    // Check if the page is dirty (swapped out)
    if (page_table[0][page].dirty) {
        // Read the page from the swap file
        tmpArr = readFromSwapFile(page);
        if (tmpArr == nullptr) {
            cleanupAndExit();
        }
        char zeros[page_size];
        memset(zeros, '0', page_size);
        // Write zeros to the swap file
        if (writeZerosToSwapFile(zeros)) {
            cleanupAndExit();
        }
        page_table[0][page].swap_index = -1;
        // Fill the page in memory and return the requested value
        return main_memory[fillPageInMemory(page, offset, tmpArr)];
    }

    // Check the type of memory access and load the page accordingly
    if (address >= text_size) {
        // Read from program file
        if (address >= text_size && address < (text_size + data_size)) {
            tmpArr = readFromProgramFile(page);
            if (tmpArr == nullptr) {
                cleanupAndExit();
            }
            // Fill the page in memory and return the requested value
            return main_memory[fillPageInMemory(page, offset, tmpArr)];
        }
        // Allocate zeros page
        if (address >= (text_size + data_size) && address < (text_size + data_size + bss_size)) {
            tmpArr = allocateZerosPage();
            if (tmpArr == nullptr) {
                cleanupAndExit();
            }
            // Fill the page in memory and return the requested value
            return main_memory[fillPageInMemory(page, offset, tmpArr)];
        }
        // Invalid access to memory
        perror("Invalid Access to Memory");
        cleanupAndExit();
    }

    // Return null character if none of the conditions are met
    return '\0';
}

void sim_mem::store(int address, char value) {
    // Check if the address is valid
    int pagesNum = (this->data_size + this->bss_size + this->heap_stack_size)/this->page_size;

    if (address >= page_size * pagesNum) {
        perror("Invalid address");
        cleanupAndExit();
    }

    char* tmpArr;
    int page = address / page_size;
    int offset = address % page_size;

    // Check if the page is already in main memory
    if (page_table[0][page].valid) {
        // Set the page as dirty and store the value
        page_table[0][page].dirty = true;
        main_memory[(page_table[0][page].frame * page_size) + offset] = value;
        return;
    }

    // Check the type of memory access and store the value accordingly
    if (address >= text_size) {
        // Read from program file
        if (address >= text_size && address < (text_size + data_size)) {
            tmpArr = readFromProgramFile(page);
            if (tmpArr == nullptr) {
                cleanupAndExit();
            }
            // Set the page as dirty and store the value in memory
            page_table[0][page].dirty = true;
            main_memory[fillPageInMemory(page, offset, tmpArr)] = value;
            return;
        }
        // Allocate zeros page
        tmpArr = allocateZerosPage();
        if (tmpArr == nullptr) {
            cleanupAndExit();
        }
        // Set the page as dirty and store the value in memory
        page_table[0][page].dirty = true;
        main_memory[fillPageInMemory(page, offset, tmpArr)] = value;
        return;
    }

    // Check if the page is dirty (swapped out)
    if (page_table[0][page].dirty) {
        // Read the page from the swap file
        tmpArr = readFromSwapFile(page);
        if (tmpArr == nullptr) {
            cleanupAndExit();
        }
        char zeros[page_size];
        memset(zeros, '0', page_size);
        // Write zeros to the swap file
        if (writeZerosToSwapFile(zeros)) {
            cleanupAndExit();
        }
        page_table[0][page].swap_index = -1;
        // Store the value in memory
        main_memory[fillPageInMemory(page, offset, tmpArr)] = value;
        return;
    }
}

char* sim_mem::readFromSwapFile(int page) {
    char* tmpArr = (char*)malloc(sizeof(char) * page_size);
    if (tmpArr == nullptr) {
        perror("Memory allocation failed");
        return nullptr;
    }
    // Set the file pointer to the swap index of the page
    if (lseek(swapfile_fd, page_table[0][page].swap_index, SEEK_SET) == -1) {
        perror("lseek() failed");
        free(tmpArr);
        return nullptr;
    }
    // Read the page from the swap file
    if (read(swapfile_fd, tmpArr, page_size) == -1) {
        perror("read() failed");
        free(tmpArr);
        return nullptr;
    }
    // Set the file pointer to the swap index of the page again for future operations
    if (lseek(swapfile_fd, page_table[0][page].swap_index, SEEK_SET) == -1) {
        perror("lseek() failed");
        free(tmpArr);
        return nullptr;
    }
    return tmpArr;
}

int sim_mem::writeZerosToSwapFile(const char* zeros) const {
    // Write zeros to the swap file
    if (write(swapfile_fd, zeros, page_size) == -1) {
        perror("write() failed");
        return -1;
    }
    return 0;
}

char* sim_mem::readFromProgramFile(int page) const {
    char* tmpArr = (char*)malloc(sizeof(char) * page_size);
    if (tmpArr == nullptr) {
        perror("Memory allocation failed");
        return nullptr;
    }
    // Set the file pointer to the starting position of the page in the program file
    lseek(program_fd, page * page_size, SEEK_SET);
    // Read the page from the program file
    if (read(program_fd, tmpArr, page_size) == -1) {
        perror("read() failed");
        free(tmpArr);
        return nullptr;
    }
    return tmpArr;
}

char* sim_mem::allocateZerosPage() const {
    char* tmpArr = (char*)malloc(sizeof(char) * page_size);
    if (tmpArr == nullptr) {
        perror("Memory allocation failed");
        return nullptr;
    }
    // Fill the page with zeros
    memset(tmpArr, '0', page_size);
    return tmpArr;
}

sim_mem::~sim_mem() {
    int i = 0;
    while (i < 1) {
        delete [] page_table[i];
        close(program_fd);
        i++;
    }
    delete [] page_table;
    delete [] frameSpace;
    close(swapfile_fd);
}



/**************************************************************************************/
void sim_mem::print_memory() {
    int i;
    printf("\nPhysical memory\n");
    for(i = 0; i < MEMORY_SIZE; i++) {
        printf("[%d] [%c]\n",i, main_memory[i]);
    }
}
/************************************************************************************/
void sim_mem::print_swap() const {
    char* str = (char*)malloc(this->page_size * sizeof(char));

    int i;
    printf("\n Swap memory\n");
    lseek(swapfile_fd, 0, SEEK_SET); // go to the start of the file
    while(read(swapfile_fd, str, this->page_size) == this->page_size) {
        for(i = 0; i < page_size; i++) {
            printf("%d - [%c]\t", i, str[i]);
        }
        printf("\n");
    }
}
/***************************************************************************************/
void sim_mem::print_page_table() {
    int i;
    int num_of_txt_pages =text_size / page_size;
    int num_of_data_pages =num_of_txt_pages+(data_size / page_size);
    int num_of_bss_pages = num_of_data_pages+(bss_size / page_size);
    int num_of_stack_heap_pages =num_of_bss_pages+(heap_stack_size / page_size);

    printf("Valid\t Dirty\t Frame\t Swap index\n");
    for(i = 0; i < num_of_txt_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[0][i].valid,
               page_table[0][i].dirty,
               page_table[0][i].frame,
               page_table[0][i].swap_index);

    }
    printf("Valid\t Dirty\t Frame\t Swap index\n");
    for(; i < num_of_data_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[0][i].valid,
               page_table[0][i].dirty,
               page_table[0][i].frame ,
               page_table[0][i].swap_index);

    }
    printf("Valid\t Dirty\t Frame\t Swap index\n");
    for(; i < num_of_bss_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[0][i].valid,
               page_table[0][i].dirty,
               page_table[0][i].frame ,
               page_table[0][i].swap_index);

    }
    printf("Valid\t Dirty\t Frame\t Swap index\n");
    for(; i < num_of_stack_heap_pages; i++) {
        printf("[%d]\t[%d]\t[%d]\t[%d]\n",
               page_table[0][i].valid,
               page_table[0][i].dirty,
               page_table[0][i].frame ,
               page_table[0][i].swap_index);
    }
}