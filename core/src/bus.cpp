#include "../include/bus.h"
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

    }

    void bus::write_16(const u32& _address, const u16& _data)
    {
        
    }

    void bus::write_8(const u32& _address, const u8& _data)
    {
        
    }

    const bool bus::debug_load_program(const std::string& _filePath)
    {
        std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
        file.unsetf(std::ios::skipws);

        if (!file.good())
            return false;

        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        programData.reserve(fileSize);
        programData.insert(programData.begin(), std::istream_iterator<u8>(file), std::istream_iterator<u8>());

        file.close();

        return programData.size() > 0;
    }
}