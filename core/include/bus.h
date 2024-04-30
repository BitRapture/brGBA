#pragma once
#include "typedefs.h"
#include "bus_constants.h"
#include <array>
#include <vector>
#include <string>
#include <unordered_map>

namespace br::gba
{
    class bus
    {
    public:
        /// @brief read 32bit data from address
        /// @param _address absolute address
        /// @return 32bit data
        const u32 read_32(const u32& _address);
        /// @brief read 16bit data from address
        /// @param _address absolute address
        /// @return 16bit data
        const u16 read_16(const u32& _address);
        /// @brief read 8bit data from address
        /// @param _address absolute address
        /// @return 8bit data
        const u8 read_8(const u32& _address);

        /// @brief write 32bit data to address
        /// @param _address absolute address
        /// @param _data 32bit data
        void write_32(const u32& _address, const u32& _data);
        /// @brief write 16bit data to address
        /// @param _address absolute address
        /// @param _data 16bit data
        void write_16(const u32& _address, const u16& _data);
        /// @brief write 8bit data to address
        /// @param _address absolute address
        /// @param _data 8bit data
        void write_8(const u32& _address, const u8& _data);

    public:
        const bool load_bios(const std::string& _filePath);

        const bool load_rom(const std::string& _filePath);

        const bool debug_load_program(const std::string& _filePath);

        const std::string debug_print_memory(const u32& _address);

    private:
        std::vector<u8> memoryBIOS;
        std::vector<u8> boardWRAM;
        std::vector<u8> chipWRAM;
        std::vector<u8> ioRegisters;
        std::vector<u8> memoryROM;
        std::vector<u8> memorySRAM;

        std::vector<u8> programData;

    public:
        bus();
    };
}