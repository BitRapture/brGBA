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
        if (_address >= programData.size())
            return 0;

        return programData[_address];
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
        if (_address >= programData.size())
            return;
        
        programData[_address] = _data;
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
}