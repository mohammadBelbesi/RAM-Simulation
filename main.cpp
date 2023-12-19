#include "sim_mem.h"
char main_memory[MEMORY_SIZE];

int main() {
    /////////////////////////////////////////////////////////////////////////////
    sim_mem s((char*)"exec_file1", (char*)"swap_file" ,25, 50, 25,25, 5);
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
    return 0;
}