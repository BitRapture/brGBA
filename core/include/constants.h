#pragma once
#include "typedefs.h"   
#include <limits>

namespace br::gba
{
    inline constexpr u32 ARM_ISA_COUNT = 3;

    inline constexpr u32 STATUS_REGISTER_N_SHIFT = 31;
    inline constexpr u32 STATUS_REGISTER_Z_SHIFT = 30;
    inline constexpr u32 STATUS_REGISTER_C_SHIFT = 29;
    inline constexpr u32 STATUS_REGISTER_V_SHIFT = 28;
    inline constexpr u32 STATUS_REGISTER_T_SHIFT = 5;

    inline constexpr u32 STATUS_REGISTER_N = 1 << STATUS_REGISTER_N_SHIFT;
    inline constexpr u32 STATUS_REGISTER_Z = 1 << STATUS_REGISTER_Z_SHIFT;
    inline constexpr u32 STATUS_REGISTER_C = 1 << STATUS_REGISTER_C_SHIFT;
    inline constexpr u32 STATUS_REGISTER_V = 1 << STATUS_REGISTER_V_SHIFT;
    inline constexpr u32 STATUS_REGISTER_T = 1 << STATUS_REGISTER_T_SHIFT;

    inline constexpr u32 ARM_DATAPROC_1_MASK = 0b0000'111'0000'0'0000'0000'00000'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_1_TEST = 0b0000'000'0000'0'0000'0000'00000'00'0'0000;
    inline constexpr u32 ARM_DATAPROC_2_MASK = 0b0000'111'0000'0'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_2_TEST = 0b0000'000'0000'0'0000'0000'0000'0'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_3_MASK = 0b0000'111'0000'0'0000'0000'0000'00000000;
    inline constexpr u32 ARM_DATAPROC_3_TEST = 0b0000'001'0000'0'0000'0000'0000'00000000;

    inline constexpr bool test_overflow_pos(const u32& _x, const u32& _y)
    {
        return (s32)_x > 0 && (s32)_y > INT32_MAX - (s32)_x;
    }

    inline constexpr bool test_overflow_neg(const u32& _x, const u32& _y)
    {
        return (s32)_x < 0 && (s32)_y > INT32_MAX + (s32)_x;
    }

    inline constexpr bool test_carry_pos(const u32& _x, const u32& _y)
    {
        u64 temp = (u64)_x + (u64)_y;
        return temp > UINT32_MAX;
    }

    inline constexpr bool test_carry_neg(const u32& _x, const u32& _y)
    {
        u64 temp = (u64)_x - (u64)_y;
        return temp > UINT32_MAX;
    }

    inline constexpr void set_bit(u32& _data, const u32& _shift, const u32& _bit)
    {
        _data = (~(1 << _shift) & _data) | (_bit << _shift);
    }

    inline constexpr bool get_bit_bool(const u32& _data, const u32& _mask)
    {
        return (_data & _mask) != 0;
    }

    inline constexpr bool get_not_bit_bool(const u32& _data, const u32& _mask)
    {
        return (_data & _mask) == 0;
    }

    inline constexpr u32 rotate_right(const u32& _data, const u32& _rotate)
    {
        return (_data >> _rotate) | (_data << ((32 - _rotate) % 32));
    }
}