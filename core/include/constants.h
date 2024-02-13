#pragma once
#include "typedefs.h"   

namespace br::gba
{
    inline constexpr u32 STATUS_REGISTER_N = 1 << 31;
    inline constexpr u32 STATUS_REGISTER_Z = 1 << 30;
    inline constexpr u32 STATUS_REGISTER_C = 1 << 29;
    inline constexpr u32 STATUS_REGISTER_V = 1 << 28;

    inline constexpr bool get_bit(const u32& _data, const u32& _mask)
    {
        return (_data & _mask) != 0;
    }

    inline constexpr bool get_not_bit(const u32& _data, const u32& _mask)
    {
        return (_data & _mask) == 0;
    }

    inline constexpr u32 condition_mask(const u32& _data)
    {
        return _data >> 28;
    }
}