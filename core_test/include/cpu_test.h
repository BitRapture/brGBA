#pragma once
#include "gba_core.h"
#include <string>
#include <vector>

namespace br::gba
{
    struct memory_region
    {
        u32 address;
        u32 count;
    };

    struct breakpoint
    {
        u32 index;
        u32 value;
    };

    class cpu_test
    {
    public:
        void run();
        void load_directives_file(const std::string& _filePath);

    private:
        void set_rom_location(const tokensIterator& _first, const tokensIterator& _last);
        void set_output_location(const tokensIterator& _first, const tokensIterator& _last);
        void set_max_cycles(const tokensIterator& _first, const tokensIterator& _last);
        void print_memory_region(const tokensIterator& _first, const tokensIterator& _last);
        void print_register_status(const tokensIterator& _first, const tokensIterator& _last);
        void set_register_breakpoint(const tokensIterator& _first, const tokensIterator& _last);
        void set_memory_breakpoint(const tokensIterator& _first, const tokensIterator& _last);
    
    private:
        std::string romFilePath;
        std::string outputFilePath;
        u32 cpuCycleMax;
        bool printRegisterStatus;
        std::vector<memory_region> printMemoryRegions;
        std::vector<breakpoint> breakpointsRegister;
        std::vector<breakpoint> breakpointsMemory;

    private:
        token_callbacks tokenCallbacks;

    public:
        cpu_test();
    };

}