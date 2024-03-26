#pragma once
#include "typedefs.h"   
#include <limits>
#include <array>
#include <string>
#include <algorithm>

namespace br::gba
{
    inline constexpr u32 ARM_ISA_COUNT = 13;

    inline constexpr u32 ARM_WORD_LENGTH = 4;
    inline constexpr u32 ARM_WORD_BIT_LENGTH = 32;
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
    inline constexpr u32 ARM_MULTIPLY_1_MASK = 0b0000'111111'00'0000'0000'0000'1111'0000;
    inline constexpr u32 ARM_MULTIPLY_1_TEST = 0b0000'000000'00'0000'0000'0000'1001'0000;
    inline constexpr u32 ARM_MULTIPLY_2_MASK = 0b0000'11111'000'0000'0000'0000'1111'0000;
    inline constexpr u32 ARM_MULTIPLY_2_TEST = 0b0000'00001'000'0000'0000'0000'1001'0000;

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

    inline const std::string bit_string(const u32& _data, const bool& _trimLeading = false, const bool& _direction = true, const u32& _length = ARM_WORD_BIT_LENGTH)
    {
        u32 leadingCount = 0;
        std::string bitString = "";
        for (u32 i = 0; i < _length; ++i)
        {
            u32 bit = get_bit_bool(_data, 1 << _length - 1 - i);
            leadingCount = bool_lerp(leadingCount + 1, 0, bool_lerp(bit, (_data >> i) & 0b1, _direction));
            bitString += bit ? "1" : "0";
        }

        leadingCount *= _trimLeading;

        bitString = bitString.substr(bool_lerp(0, leadingCount, _direction), 
                        bool_lerp(bitString.length() - leadingCount, bitString.length(), _direction));
        return bitString;
    }

    template <typename T, u32 S, u32 SB>
    inline constexpr void sort_isa_array(std::array<T, S>& _isaArray)
    {
        std::array<u32, S> isaIndices;
        std::array<u32, S> isaMaskCount;
        std::array<u32, S> isaTestCount;
        for (u32 i = 0; i < S; ++i)
        {
            isaIndices[i] = i;
            isaMaskCount[i] = bit_count(_isaArray[i].data_mask, SB);
            isaTestCount[i] = bit_count(_isaArray[i].data_test, SB);
        }

        for (u32 i = 0; i < S; ++i)
        {
            for (u32 j = 0; j < S; ++j)
            {                
                if (isaIndices[i] == isaIndices[j])
                    continue;

                u32 tempIndex, tempMaskCount, tempTestCount;
                bool maskCountGreater = isaMaskCount[i] > isaMaskCount[j];
                bool maskGreater = _isaArray[isaIndices[i]].data_mask > _isaArray[isaIndices[j]].data_mask;

                if (maskCountGreater || maskGreater)
                {
                    tempMaskCount = isaMaskCount[i];
                    tempTestCount = isaTestCount[i];
                    tempIndex = isaIndices[i];

                    isaMaskCount[i] = isaMaskCount[j];
                    isaTestCount[i] = isaTestCount[j];
                    isaIndices[i] = isaIndices[j];

                    isaMaskCount[j] = tempMaskCount;
                    isaTestCount[j] = tempTestCount;
                    isaIndices[j] = tempIndex;
                }
            }
        } 

        for (u32 i = 0; i < S; ++i)
        {
            for (u32 j = 0; j < S; ++j)
            {                
                if (isaIndices[i] == isaIndices[j])
                    continue;

                u32 tempIndex, tempMaskCount, tempTestCount;
                bool maskEqual = _isaArray[isaIndices[i]].data_mask == _isaArray[isaIndices[j]].data_mask;
                bool testCountGreater = isaTestCount[i] > isaTestCount[j];
                bool testCountEqual = isaTestCount[i] == isaTestCount[j];
                bool testGreater = _isaArray[isaIndices[i]].data_test > _isaArray[isaIndices[j]].data_test;

                if (maskEqual && (testCountGreater || (testCountEqual && testGreater)))
                {
                    tempMaskCount = isaMaskCount[i];
                    tempTestCount = isaTestCount[i];
                    tempIndex = isaIndices[i];

                    isaMaskCount[i] = isaMaskCount[j];
                    isaTestCount[i] = isaTestCount[j];
                    isaIndices[i] = isaIndices[j];

                    isaMaskCount[j] = tempMaskCount;
                    isaTestCount[j] = tempTestCount;
                    isaIndices[j] = tempIndex;
                }
            }
        } 

        std::array<T, S> tempList;
        for (u32 i = 0; i < S; ++i)
        {
            tempList[i] = _isaArray[isaIndices[i]];
        }

        _isaArray.swap(tempList);
    }
}