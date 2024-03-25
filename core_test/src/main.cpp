#include "gba_core.h"
#include <iostream>

int main()
{
    br::gba::bus gbaBus;
    br::gba::cpu gbaCPU(gbaBus);

    if (!gbaBus.debug_load_program("../core_test/src/test.o"))
    {
        std::cout << "Could not load test ROM";
        return -1;
    }

    std::cout << gbaCPU.debug_print_isa(true);
    
    for (br::u32 i = 0; i < 48; ++i)
        gbaCPU.cycle();
    
    gbaCPU.debug_save_log("./cpu.log");
    
    //for (br::u32 i = 0; i < 10; ++i)
    //    std::cout << gbaBus.debug_print_memory(0xFF0000 + (i * 4));

    return 0;
}