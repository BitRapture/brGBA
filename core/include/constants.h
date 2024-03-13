#pragma once
#include "typedefs.h"   

namespace br::gba
{
    inline constexpr u32 ARM_ISA_COUNT = 3;

    inline constexpr u32 STATUS_REGISTER_N = 1 << 31;
    inline constexpr u32 STATUS_REGISTER_Z = 1 << 30;
    inline constexpr u32 STATUS_REGISTER_C = 1 << 29;
    inline constexpr u32 STATUS_REGISTER_V = 1 << 28;
    inline constexpr u32 STATUS_REGISTER_T = 1 << 5;

    inline constexpr u32 ARM_DATAPROC_1_MASK = 0b0000'111'0000'0'0000'0000'00000'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_1_TEST = 0b0000'000'0000'0'0000'0000'00000'00'0'0000;
    inline constexpr u32 ARM_DATAPROC_2_MASK = 0b0000'111'0000'0'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_2_TEST = 0b0000'000'0000'0'0000'0000'0000'0'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_3_MASK = 0b0000'111'0000'0'0000'0000'0000'00000000;
    inline constexpr u32 ARM_DATAPROC_3_TEST = 0b0000'001'0000'0'0000'0000'0000'00000000;

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

    inline constexpr u32 rotate_right(const u32& _data, const u32& _rotate)
    {
        return (_data >> _rotate) | (_data << ((32 - _rotate) % 32));
    }
}