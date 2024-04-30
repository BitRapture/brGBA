#pragma once
#include "typedefs.h"
#include <vector>   

namespace br::gba
{
    inline constexpr u32 MEMORY_BIOS_SIZE = 0x4000;
    inline constexpr u32 MEMORY_BOARD_WRAM_SIZE = 0x40000;
    inline constexpr u32 MEMORY_CHIP_WRAM_SIZE = 0x8000;
    inline constexpr u32 MEMORY_IO_REGISTERS_SIZE = 0x3FF;
    inline constexpr u32 MEMORY_ROM_SIZE = 0x2000000;
    inline constexpr u32 MEMORY_ROM_TOTAL_SIZE = MEMORY_ROM_SIZE * 3;
    inline constexpr u32 MEMORY_SRAM_SIZE = 0x10000;

    inline constexpr u32 MEMORY_BIOS_ADDR = 0x0;
    inline constexpr u32 MEMORY_BOARD_WRAM_ADDR = 0x2000000;
    inline constexpr u32 MEMORY_CHIP_WRAM_ADDR = 0x3000000;
    inline constexpr u32 MEMORY_IO_REGISTERS_ADDR = 0x4000000;
    inline constexpr u32 MEMORY_ROM_0_ADDR = 0x8000000;
    inline constexpr u32 MEMORY_ROM_1_ADDR = 0xA000000;
    inline constexpr u32 MEMORY_ROM_2_ADDR = 0xC000000;
    inline constexpr u32 MEMORY_SRAM_ADDR = 0xE000000;

    template<std::size_t S, u32 A>
    bool test_address_region(const u32& _address, u32& _relativeAddress)
    {
        bool isInRange = _address >= A && _address < A + S;
        if (isInRange)
            _relativeAddress = _address - A;
        return isInRange;
    }

    template<std::size_t S, u32 A>
    bool write_memory(std::vector<u8>& _memArray, const u32& _address, const u8& _data)
    {
        bool isInRange = _address >= A && _address < A + S;
        u32 relativeAddress = _address - A;
        if (isInRange)
            _memArray[relativeAddress] = _data;

        return isInRange;
    }
}