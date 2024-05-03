#include "../include/cpu_test.h"
#include <string>
#include <iterator>
#include <fstream>
#include <iostream>
#include <chrono>

namespace br::gba
{
    void cpu_test::run()
    {
        std::chrono::high_resolution_clock timer;
        br::gba::bus gbaBus;
        br::gba::cpu gbaCPU(gbaBus);

        gbaBus.write_32(0x0, 0xE3A00302);
        gbaBus.write_32(0x4, 0xE12FFF10);

        if (!gbaBus.load_rom(romFilePath))
        {
            std::cout << "Could not load test ROM" << std::endl;
            return;
        }

        gbaCPU.reset();

        u32 i = 0;
        bool isBreak = false;
        auto cpuStart = timer.now();
        while ((cpuCycleMax == 0 || i <= cpuCycleMax) && !isBreak)
        {
            gbaCPU.cycle();

            for (const breakpoint& breakPoint : breakpointsRegister)
            {
                isBreak |= gbaCPU.debug_get_register(breakPoint.index) == breakPoint.value;
            }

            for (const breakpoint& breakPoint : breakpointsMemory)
            {
                isBreak |= gbaBus.read_32(breakPoint.index) == breakPoint.value;
            }

            i++;
        }
        auto cpuEnd = timer.now();

        for (const memory_region& region : printMemoryRegions)
        {
            for (u32 i = 0; i < region.count; ++i)
                std::cout << gbaBus.debug_print_memory(region.address + (i * 4));
        }

        if (printRegisterStatus)
            std::cout << gbaCPU.debug_print_status();

        auto totalMills = std::chrono::duration_cast<std::chrono::milliseconds>(cpuEnd - cpuStart).count();
        std::cout << "Total cycles: " << i << ", Time taken: " << totalMills << "ms" << std::endl;
        
        if (outputFilePath.length() > 0)
            gbaCPU.debug_save_log(outputFilePath);
    }

    void cpu_test::load_directives_file(const std::string& _filePath)
    {
        std::ifstream file(_filePath);

        if (!file.good())
            return;
        
        std::string fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        tokens fileTokens = tokenize(fileData);
        if (!parse_tokens(fileTokens, tokenCallbacks))
            std::cout << "Tokens not parsed correctly" << std::endl;

        file.close();
    }

    void cpu_test::set_rom_location(const tokensIterator& _first, const tokensIterator& _last)
    {
        romFilePath = *_last;
    }

    void cpu_test::set_output_location(const tokensIterator& _first, const tokensIterator& _last)
    {
        outputFilePath = *_last;
    }

    void cpu_test::set_max_cycles(const tokensIterator& _first, const tokensIterator& _last)
    {
        cpuCycleMax = std::stol(*_last);
    }

    void cpu_test::print_memory_region(const tokensIterator& _first, const tokensIterator& _last)
    {
        memory_region memoryRegion;
        memoryRegion.address = std::stol(*(_last - 1));
        memoryRegion.count = std::stol(*_last);
        printMemoryRegions.push_back(memoryRegion);
    }

    void cpu_test::print_register_status(const tokensIterator& _first, const tokensIterator& _last)
    {
        printRegisterStatus = true;
    }

    void cpu_test::set_register_breakpoint(const tokensIterator& _first, const tokensIterator& _last)
    {
        breakpoint breakPoint;
        breakPoint.index = std::stol(*(_last - 1));
        breakPoint.value = std::stol(*_last);
        breakpointsRegister.push_back(breakPoint);
    }

    void cpu_test::set_memory_breakpoint(const tokensIterator& _first, const tokensIterator& _last)
    {
        breakpoint breakPoint;
        breakPoint.index = std::stol(*(_last - 1));
        breakPoint.value = std::stol(*_last);
        breakpointsMemory.push_back(breakPoint);
    }

    cpu_test::cpu_test()
    {
        tokenCallbacks = 
        {
            { "rom", 1, std::bind(&cpu_test::set_rom_location, this, std::placeholders::_1, std::placeholders::_2) },
            { "out", 1, std::bind(&cpu_test::set_output_location, this, std::placeholders::_1, std::placeholders::_2) },
            { "cycles", 1, std::bind(&cpu_test::set_max_cycles, this, std::placeholders::_1, std::placeholders::_2) },
            { "memdump", 2, std::bind(&cpu_test::print_memory_region, this, std::placeholders::_1, std::placeholders::_2) },
            { "regdump", 0, std::bind(&cpu_test::print_register_status, this, std::placeholders::_1, std::placeholders::_2) },
            { "regbreak", 2, std::bind(&cpu_test::set_register_breakpoint, this, std::placeholders::_1, std::placeholders::_2) },
            { "membreak", 2, std::bind(&cpu_test::set_memory_breakpoint, this, std::placeholders::_1, std::placeholders::_2) }
        };
    }
}