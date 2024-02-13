#pragma once
#include "typedefs.h"

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
    };
}