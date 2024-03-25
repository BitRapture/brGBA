#pragma once
#include "typedefs.h"   
#include <limits>

namespace br::gba
{
    inline constexpr u32 ARM_ISA_COUNT = 11;

    inline constexpr u32 ARM_WORD_LENGTH = 4;
    inline constexpr u32 THUMB_WORD_LENGTH = 2;

    inline constexpr u32 REGISTER_LINK_INDEX = 14;
    inline constexpr u32 REGISTER_PROGRAM_COUNTER_INDEX = 15;

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
    inline constexpr u32 REGISTER_LIST_LENGTH = 16;

    inline constexpr u32 ARM_CONDITION_SHIFT = 28;
    
    inline constexpr u32 ARM_DATAPROC_1_MASK = 0b0000'111'0000'0'0000'0000'00000'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_1_TEST = 0b0000'000'0000'0'0000'0000'00000'00'0'0000;
    inline constexpr u32 ARM_DATAPROC_2_MASK = 0b0000'111'0000'0'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_2_TEST = 0b0000'000'0000'0'0000'0000'0000'0'00'1'0000;
    inline constexpr u32 ARM_DATAPROC_3_MASK = 0b0000'111'0000'0'0000'0000'0000'00000000;
    inline constexpr u32 ARM_DATAPROC_3_TEST = 0b0000'001'0000'0'0000'0000'0000'00000000;
    inline constexpr u32 ARM_BRANCHING_1_MASK = 0b0000'1111111111111111111111'0'1'0000;
    inline constexpr u32 ARM_BRANCHING_1_TEST = 0b0000'0001001011111111111100'0'1'0000;
    inline constexpr u32 ARM_BRANCHING_2_MASK = 0b0000'111'0'000000000000000000000000;
    inline constexpr u32 ARM_BRANCHING_2_TEST = 0b0000'101'0'000000000000000000000000;
    inline constexpr u32 ARM_TRANSFER_1_MASK = 0b0000'111'00000'0000'0000'00000'00'1'0000;
    inline constexpr u32 ARM_TRANSFER_1_TEST = 0b0000'011'00000'0000'0000'00000'00'0'0000;
    inline constexpr u32 ARM_TRANSFER_2_MASK = 0b0000'111'00000'0000'0000'000000000000;
    inline constexpr u32 ARM_TRANSFER_2_TEST = 0b0000'010'00000'0000'0000'000000000000;
    inline constexpr u32 ARM_TRANSFER_3_MASK = 0b0000'111'00'1'00'0000'0000'1111'1'00'1'0000;
    inline constexpr u32 ARM_TRANSFER_3_TEST = 0b0000'000'00'0'00'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_TRANSFER_4_MASK = 0b0000'111'00'1'00'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_TRANSFER_4_TEST = 0b0000'000'00'1'00'0000'0000'0000'1'00'1'0000;
    inline constexpr u32 ARM_TRANSFER_5_MASK = 0b0000'11111'0'11'0000'0000'1111'1111'0000;
    inline constexpr u32 ARM_TRANSFER_5_TEST = 0b0000'00010'0'00'0000'0000'0000'1001'0000;
    inline constexpr u32 ARM_TRANSFER_6_MASK = 0b0000'111'00000'0000'0000000000000000;
    inline constexpr u32 ARM_TRANSFER_6_TEST = 0b0000'100'00000'0000'0000000000000000;

    inline constexpr bool test_overflow_pos(const u32& _x, const u32& _y)
    {
        s64 temp = (s64)_x + (s64)_y; 
        return temp < INT32_MIN || temp > INT32_MAX;
    }

    inline constexpr bool test_overflow_neg(const u32& _x, const u32& _y)
    {
        s64 temp = (s64)_x - (s64)_y; 
        return temp < INT32_MIN || temp > INT32_MAX;
    }

    inline constexpr bool test_overflow_pos(const u32& _x, const u32& _y, const u32& _carry)
    {
        s64 temp = (s64)_x + (s64)_y + (s64)_carry; 
        return temp < INT32_MIN || temp > INT32_MAX;
    }

    inline constexpr bool test_overflow_neg(const u32& _x, const u32& _y, const u32& _carry)
    {
        s64 temp = (s64)_x - (s64)_y + (s64)_carry - 1; 
        return temp < INT32_MIN || temp > INT32_MAX;
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

    inline constexpr bool test_carry_pos(const u32& _x, const u32& _y, const u32& _carry)
    {
        u64 temp = (u64)_x + (u64)_y + (u64)_carry;
        return temp > UINT32_MAX;        
    }

    inline constexpr bool test_carry_neg(const u32& _x, const u32& _y, const u32& _carry)
    {
        u64 temp = (u64)_x - (u64)_y + (u64)_carry - 1;
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

    inline constexpr u32 sub_or_add(const u32& _data, const u32& _operand, const u32& _bool)
    {
        return _data + (_operand * _bool) - (_operand * !_bool);
    }

    inline constexpr u32 bool_lerp(const u32& _a, const u32& _b, const u32& _bool)
    {
        return (_a * !_bool) + (_b * _bool); 
    }

    inline constexpr u32 bit_count(const u32& _data, const u32& _length)
    {
        u32 count = 0;
        for (u32 i = 0; i < _length; ++i)
        {
            count += (_data >> i) & 0b1;
        }
        return count;
    }
}