#include "gba_core.h"
#include <iostream>

int main()
{
    br::gba::bus gbaBus;
    br::gba::cpu gbaCPU(gbaBus);

    gbaCPU.debug_cycle_instruction(0xe3a00cff);
    gbaCPU.debug_cycle_instruction(0xe380003e);
    
    std::cout << gbaCPU.debug_print_status();

    return 0;
}