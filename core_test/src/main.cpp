#include "gba_core.h"
#include <iostream>

int main()
{
    br::gba::bus gbaBus;
    br::gba::cpu gbaCPU(gbaBus);

    if (!gbaBus.load_bios("bios.rom"))
    {
        std::cout << "Could not load bios ROM";
        return -1;
    }
    
    gbaBus.write_32(0x0, 0xE3A00302);
    gbaBus.write_32(0x4, 0xE12FFF10);

    if (!gbaBus.load_rom("test.o"))
    {
        std::cout << "Could not load test ROM";
        return -1;
    }

    gbaCPU.reset();

    std::cout << gbaCPU.debug_print_isa(true) << '\n';
    std::cout << gbaCPU.debug_print_isa(false);
    
    for (br::u32 i = 0; i < 48; ++i)
        gbaCPU.cycle();
    
    gbaCPU.debug_save_log("./cpu.log");
    
    //for (br::u32 i = 0; i < 10; ++i)
    //    std::cout << gbaBus.debug_print_memory(0xFF0000 + (i * 4));

    return 0;
}