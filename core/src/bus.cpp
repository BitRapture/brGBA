#include "../include/bus.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iterator>

namespace br::gba
{
    const u32 bus::read_32(const u32& _address)
    {
        u32 lo = read_16(_address);
        u32 hi = read_16(_address + 2) << 16;
        return hi | lo;
    }

    const u16 bus::read_16(const u32& _address)
    {
        u16 lo = read_8(_address);
        u16 hi = read_8(_address + 1) << 8;
        return hi | lo;
    }

    const u8 bus::read_8(const u32& _address)
    {
        u32 relativeAdress = _address;

        if (test_address_region<MEMORY_BIOS_SIZE, MEMORY_BIOS_ADDR>(_address, relativeAdress))
            return memoryBIOS[relativeAdress];

        if (test_address_region<MEMORY_BOARD_WRAM_SIZE, MEMORY_BOARD_WRAM_ADDR>(_address, relativeAdress))
            return boardWRAM[relativeAdress];    
        
        if (test_address_region<MEMORY_CHIP_WRAM_SIZE, MEMORY_CHIP_WRAM_ADDR>(_address, relativeAdress))
            return chipWRAM[relativeAdress];

        if (test_address_region<MEMORY_IO_REGISTERS_SIZE, MEMORY_IO_REGISTERS_ADDR>(_address, relativeAdress))
            return ioRegisters[relativeAdress];

        if (test_address_region<MEMORY_ROM_TOTAL_SIZE, MEMORY_ROM_0_ADDR>(_address, relativeAdress))
            return memoryROM[relativeAdress];

        if (test_address_region<MEMORY_SRAM_SIZE, MEMORY_SRAM_ADDR>(_address, relativeAdress))
            return memorySRAM[relativeAdress];        

        return 0;    
    }

    void bus::write_32(const u32& _address, const u32& _data)
    {
        write_16(_address, _data & 0xFFFF);
        write_16(_address + 2, _data >> 16);
    }

    void bus::write_16(const u32& _address, const u16& _data)
    {
        write_8(_address, _data & 0xFF);
        write_8(_address + 1, _data >> 8);
    }

    void bus::write_8(const u32& _address, const u8& _data)
    {
        if (write_memory<MEMORY_BIOS_SIZE, MEMORY_BIOS_ADDR>(memoryBIOS, _address, _data))
            return;

        if (write_memory<MEMORY_BOARD_WRAM_SIZE, MEMORY_BOARD_WRAM_ADDR>(boardWRAM, _address, _data))
            return;

        if (write_memory<MEMORY_CHIP_WRAM_SIZE, MEMORY_CHIP_WRAM_ADDR>(chipWRAM, _address, _data))
            return;

        if (write_memory<MEMORY_IO_REGISTERS_SIZE, MEMORY_IO_REGISTERS_ADDR>(ioRegisters, _address, _data))
            return;

        if (write_memory<MEMORY_ROM_TOTAL_SIZE, MEMORY_ROM_0_ADDR>(memoryROM, _address, _data))
            return;

        if (write_memory<MEMORY_SRAM_SIZE, MEMORY_SRAM_ADDR>(memorySRAM, _address, _data))
            return;
    }

    const bool bus::load_bios(const std::string& _filePath)
    {
        std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
        file.unsetf(std::ios::skipws);

        if (!file.good())
            return false;

        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize > MEMORY_BIOS_SIZE)
            return false;

        file.read(reinterpret_cast<char*>(memoryBIOS.data()), fileSize);
        file.close();

        return true;
    }

    const bool bus::load_rom(const std::string& _filePath)
    {
        std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
        file.unsetf(std::ios::skipws);

        if (!file.good())
            return false;

        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        if (fileSize > MEMORY_ROM_TOTAL_SIZE)
            return false;

        file.read(reinterpret_cast<char*>(memoryROM.data()), fileSize);
        file.close();

        return true;
    }

    const bool bus::debug_load_program(const std::string& _filePath)
    {
        std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
        file.unsetf(std::ios::skipws);

        if (!file.good())
            return false;

        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        programData.resize(0xFFFFFF, 0);
        programData.insert(programData.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());

        file.close();

        return programData.size() > 0;
    }

    const std::string bus::debug_print_memory(const u32& _address)
    {
        std::stringstream statusInfo;
        statusInfo << "Mem [0x" << std::setfill('0') << std::setw(8) << std::hex << _address << "]: 0x";
        statusInfo << std::setfill('0') << std::setw(8) << std::hex << read_32(_address) << "\n";
        return statusInfo.str();
    }

    bus::bus()
    {
        memoryBIOS.resize(MEMORY_BIOS_SIZE, 0);
        boardWRAM.resize(MEMORY_BOARD_WRAM_SIZE, 0);
        chipWRAM.resize(MEMORY_CHIP_WRAM_SIZE, 0);
        ioRegisters.resize(MEMORY_IO_REGISTERS_SIZE, 0);
        memoryROM.resize(MEMORY_ROM_TOTAL_SIZE, 0);
        memorySRAM.resize(MEMORY_SRAM_SIZE, 0);
    }
}