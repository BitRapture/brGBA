#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/constants.h"

namespace br::gba
{
    const u32 cpu::cycle()
    {
        if (get_bit(statusRegister, STATUS_REGISTER_T))
            return decode_arm_instruction();
        else
            return decode_thumb_instruction();
    }

    const u32 cpu::debug_cycle_instruction(const u32& _instruction)
    {
        for (u32 i = 0; i < ARM_ISA_COUNT; ++i)
        {
            cpu_instruction currentInstruction = armISA[i];

            if ((currentInstruction.data_mask & _instruction) == currentInstruction.data_test)
                return currentInstruction.execute(_instruction);
        }

        return 0;
    }

    const std::string cpu::debug_print_status()
    {
        std::string statusInfo = "";

        for (u32 i = 0; i <= 0xF; ++i)
        {
            statusInfo += "Register " + std::to_string(i) + ": " + std::to_string(get_register(i)) + "\n";
        }

        return statusInfo;
    }

    const u32 cpu::decode_arm_instruction()
    {
        u32 opcode = addressBus.read_32(programCounter);
        programCounter += 4;

        for (u32 i = 0; i < ARM_ISA_COUNT; ++i)
        {
            cpu_instruction currentInstruction = armISA[i];

            if ((currentInstruction.data_mask & opcode) == currentInstruction.data_test)
                return currentInstruction.execute(opcode);
        }

        return 0;
    }

    const u32 cpu::decode_thumb_instruction()
    {
        u16 opcode = addressBus.read_16(programCounter);
        programCounter += 2;

        return 0;
    }

    u32& cpu::get_register(const u32& _index)
    {
        switch (_index)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            return thumbRegisters[_index];
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            return armRegisters[armRegisterOffset + (_index - 8)];
        case 13:
            return stackPointers[bankedRegisterOffset];
        case 14:
            return linkRegisters[bankedRegisterOffset];
        case 15:
            return programCounter;
        }

        // shouldn't ever manage to get past 15
        return programCounter;
    }

    const bool cpu::check_condition(const u32& _code)
    {
        switch (_code)
        {
        case 0: // EQ
            return get_bit(statusRegister, STATUS_REGISTER_Z);
        case 1: // NE
            return get_not_bit(statusRegister, STATUS_REGISTER_Z);
        case 2: // CS/HS
            return get_bit(statusRegister, STATUS_REGISTER_C);
        case 3: // CC/LO
            return get_not_bit(statusRegister, STATUS_REGISTER_C);
        case 4: // MI
            return get_bit(statusRegister, STATUS_REGISTER_N);
        case 5: // PL
            return get_not_bit(statusRegister, STATUS_REGISTER_N);
        case 6: // VS
            return get_bit(statusRegister, STATUS_REGISTER_V);
        case 7: // VC
            return get_not_bit(statusRegister, STATUS_REGISTER_V);
        case 8: // HI
            return get_bit(statusRegister, STATUS_REGISTER_C) && get_not_bit(statusRegister, STATUS_REGISTER_Z);
        case 9: // LS
            return get_not_bit(statusRegister, STATUS_REGISTER_C) || get_bit(statusRegister, STATUS_REGISTER_Z);
        case 10: // GE
            return get_not_bit(statusRegister, STATUS_REGISTER_N) == get_not_bit(statusRegister, STATUS_REGISTER_V);
        case 11: // LT
            return get_not_bit(statusRegister, STATUS_REGISTER_N) != get_not_bit(statusRegister, STATUS_REGISTER_V);
        case 12: // GT
            return get_not_bit(statusRegister, STATUS_REGISTER_Z) && (get_not_bit(statusRegister, STATUS_REGISTER_N) == get_not_bit(statusRegister, STATUS_REGISTER_V));
        case 13: // LE
            return get_bit(statusRegister, STATUS_REGISTER_Z) && (get_not_bit(statusRegister, STATUS_REGISTER_N) != get_not_bit(statusRegister, STATUS_REGISTER_V));
        case 14: // AL
            return true;
        case 15: // NV
            return false;
        }

        return false;
    }

    const u32 cpu::arm_dataproc(const u32& _opcode)
    {
        bool isImmediate = get_bit(_opcode, 1 << 25);
        bool setConditions = get_bit(_opcode, 1 << 20);

        u32 dataOpcode = (_opcode >> 21) & 0b1111;
        u32& regN = get_register((_opcode >> 16) & 0b1111);
        u32& regD = get_register((_opcode >> 12) & 0b1111);

        u32 operand = 0;
        u32 carry = 0;
        if (isImmediate)
        {
            u32 immediate = _opcode & 0xFF;
            u32 rotate = ((_opcode >> 8) & 0b1111) << 1;
            
            operand = rotate_right(immediate, rotate);
            carry = immediate & 0b1;
        }
        else
        {
            bool shiftRegister = get_bit(_opcode, 1 << 4);
            bool zeroShift = false;
            bool zeroShiftRegister = false;
            u32 shiftType = (_opcode >> 5) & 0b11;
            
            u32 shift;
            if (shiftRegister)
            {
                shift = get_register((_opcode >> 8) & 0b1111) & 0xFF;
                zeroShiftRegister = shift == 0;
            }
            else
            {
                shift = (_opcode >> 7) & 0b11111;
                zeroShift = shift == 0;

                if (zeroShift)
                    shift = 32;
            }

            operand = get_register(_opcode & 0b1111);

            switch (shiftType)
            {
            case 0x0: // LSL
                if (zeroShift)
                    break;
                carry = (operand << shift - 1) >> 31;
                operand <<= shift;
                break;
            case 0x1: // LSR
                carry = (operand >> shift - 1) & 0b1;
                operand >>= shift;
                break;
            case 0x2: // ASR
                // compiler must be able to perform asr for this to work correctly!!
                carry = ((s32)operand >> shift - 1) & 0b1;
                operand = (s32)operand >> shift; 
                break;
            case 0x3: // ROR
                carry = operand & 0b1;
                if (zeroShift) // RCR
                {
                    operand >>= 1;
                    operand |= (u32)get_bit(statusRegister, STATUS_REGISTER_C) >> 31;
                }
                else
                {
                    operand = rotate_right(operand, shift);
                }
                break;
            }
        }

        u32 overflow = 0;
        u32 zero = 0;
        u32 sign = 0;
        switch (dataOpcode)
        {
        case 0x0: // AND
            regD = regN & operand;
            break;
        case 0x1: // EOR
            regD = regN ^ operand;
            break;
        case 0x2: // SUB
            regD = regN - operand;
            break;
        case 0x3: // RSB
            regD = operand - regN;
            break;
        case 0x4: // ADD
            regD = regN + operand;
            break;
        case 0x5: // ADC
            regD = regN + operand + ((u32)get_bit(statusRegister, STATUS_REGISTER_C) >> 31);
            break;
        case 0x6: // SBC
            regD = regN - operand + ((u32)get_bit(statusRegister, STATUS_REGISTER_C) >> 31) - 1;
            break;
        case 0x7: // RSC
            regD = operand - regN + ((u32)get_bit(statusRegister, STATUS_REGISTER_C) >> 31) - 1;
            break;
        case 0x8: // TST
            {
                u32 result = regN & operand;
            }
            break;
        case 0x9: // TEQ
            {
                u32 result = regN ^ operand;
            }
            break;
        case 0xA: // CMP
            {
                u32 result = regN - operand;
            }
            break;
        case 0xB: // CMN
            {
                u32 result = regN + operand;
            }
            break;
        case 0xC: // ORR
            regD = regN | operand;
            break;
        case 0xD: // MOV
            regD = operand;
            break;
        case 0xE: // BIC
            regD = regN & ~operand;
            break;
        case 0xF: // MVN
            regD = ~operand;
            break;
        }

        return 0;
    }

    void cpu::create_arm_isa()
    {
        armISA[0] = { ARM_DATAPROC_1_MASK, ARM_DATAPROC_1_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1) };
        armISA[1] = { ARM_DATAPROC_2_MASK, ARM_DATAPROC_2_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1) };
        armISA[2] = { ARM_DATAPROC_3_MASK, ARM_DATAPROC_3_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1) };
    }

    cpu::cpu(bus& _addressBus)
        : addressBus{ _addressBus },
        bankedRegisterOffset{ 0 }, armRegisterOffset{ 0 }
    {
        create_arm_isa();
    }
}