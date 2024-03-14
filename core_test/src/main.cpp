#include "gba_core.h"
#include <iostream>

int main()
{
    br::gba::bus gbaBus;
    br::gba::cpu gbaCPU(gbaBus);

    gbaCPU.debug_cycle_instruction(0xe3e00000);
    gbaCPU.debug_cycle_instruction(0xe3a0100a);
    gbaCPU.debug_cycle_instruction(0xe2900001);
    gbaCPU.debug_cycle_instruction(0xe2d11001);
    
    std::cout << gbaCPU.debug_print_status();

    return 0;
}