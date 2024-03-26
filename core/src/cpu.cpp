#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/constants.h"
#include <sstream>
#include <iomanip>
#include <fstream>

namespace br::gba
{
    const u32 cpu::cycle()
    {
        if (get_bit_bool(statusRegister, STATUS_REGISTER_T))
            return decode_thumb_instruction();
        else
            return decode_arm_instruction();
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
        std::stringstream statusInfo;

        for (u32 i = 0; i <= 0xF; ++i)
        {
            statusInfo << "Register " + std::to_string(i) + ": 0x";
            statusInfo << std::setfill('0') << std::setw(8) << std::hex << get_register(i);
            statusInfo << '\n';
        }
        statusInfo << "CSPR: 0x" << std::setfill('0') << std::setw(8) << std::hex << statusRegister;

        return statusInfo.str();
    }

    void cpu::debug_save_log(const std::string& _filePath)
    {
        std::ofstream file(_filePath);

        if (!file.good())
            return;

        file << debugLog;

        file.close();
    }

    void cpu::debug_log_arm_cycle(const u32& _opcode, const cpu_instruction& _instruction)
    {
        if (_instruction.data_test == 0x0)
            return;

        std::stringstream statusInfo;
        statusInfo << "Opcode: 0x" << std::setfill('0') << std::setw(8) << std::hex << _opcode;
        statusInfo << "\nInst Test: 0x" << std::setfill('0') << std::setw(8) << std::hex << _instruction.data_test;
        statusInfo << "\nInst Type: " << _instruction.debug_info;

        debugLog += '\n';
        debugLog += statusInfo.str() + '\n';
        debugLog += debug_print_status() + '\n';
    }

    const std::string cpu::debug_print_isa(const bool& _armISA)
    {
        u32 isaSize = _armISA ? ARM_ISA_COUNT : 0;
        std::string isaList;

        for (u32 i = 0; i < isaSize; ++i)
        {
            std::string debugInfo = (_armISA ? armISA[i] : armISA[i]).debug_info;
            std::string testMask = bit_string((_armISA ? armISA[i] : armISA[i]).data_test, true, false);
            isaList += "i: " + std::to_string(i) + ", " + debugInfo + ", " + testMask + "\n";
        }
         
        return isaList;;
    }

    const u32 cpu::decode_arm_instruction()
    {
        u32 opcode = addressBus.read_32(programCounter);
        programCounter += ARM_WORD_LENGTH;

        for (u32 i = 0; i < ARM_ISA_COUNT; ++i)
        {
            cpu_instruction currentInstruction = armISA[i];

            if ((currentInstruction.data_mask & opcode) == currentInstruction.data_test)
            {
                u32 cycleCount = 0;
                cycleCount = currentInstruction.execute(opcode);
                debug_log_arm_cycle(opcode, currentInstruction);
                return cycleCount;
            }
        }

        debug_log_arm_cycle(opcode, {});

        return 0;
    }

    const u32 cpu::decode_thumb_instruction()
    {
        u16 opcode = addressBus.read_16(programCounter);
        programCounter += THUMB_WORD_LENGTH;

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

    u32& cpu::get_current_spsr(bool& _isUserMode)
    {
        cpu_mode mode = get_current_mode();
        if (mode == cpu_mode::USER || mode == cpu_mode::SYSTEM)
        {
            _isUserMode = true;
            return statusRegister;
        }

        return savedStatusRegisters[(u32)mode];
    }

    const cpu_mode cpu::get_current_mode()
    {
        u32 modeType = statusRegister & 0b11111;
        switch (modeType)
        {
        case 0b10000: // User
            return cpu_mode::USER;
        case 0b10001: // FIQ
            return cpu_mode::FIQ;
        case 0b10010: // IRQ
            return cpu_mode::IRQ;
        case 0b10011: // SWI
            return cpu_mode::SWI;
        case 0b10111: // Abort
            return cpu_mode::ABORT;
        case 0b11011: // Undefined
            return cpu_mode::UNDEFINED;
        case 0b11111: // Privelleged User
            return cpu_mode::SYSTEM;
        }

        u32 compatModeType = statusRegister & 0b10011;
        switch (modeType)
        {
        case 0b00000: // User
            return cpu_mode::USER;
        case 0b00001: // FIQ
            return cpu_mode::FIQ;
        case 0b00010: // IRQ
            return cpu_mode::IRQ;
        case 0b00011: // SWI
            return cpu_mode::SWI;
        }

        return cpu_mode::UNDEFINED;
    }

    const bool cpu::check_condition(const u32& _code)
    {
        switch (_code)
        {
        case 0: // EQ
            return get_bit_bool(statusRegister, STATUS_REGISTER_Z);
        case 1: // NE
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_Z);
        case 2: // CS/HS
            return get_bit_bool(statusRegister, STATUS_REGISTER_C);
        case 3: // CC/LO
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_C);
        case 4: // MI
            return get_bit_bool(statusRegister, STATUS_REGISTER_N);
        case 5: // PL
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_N);
        case 6: // VS
            return get_bit_bool(statusRegister, STATUS_REGISTER_V);
        case 7: // VC
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_V);
        case 8: // HI
            return get_bit_bool(statusRegister, STATUS_REGISTER_C) && get_not_bit_bool(statusRegister, STATUS_REGISTER_Z);
        case 9: // LS
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_C) || get_bit_bool(statusRegister, STATUS_REGISTER_Z);
        case 10: // GE
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_N) == get_not_bit_bool(statusRegister, STATUS_REGISTER_V);
        case 11: // LT
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_N) != get_not_bit_bool(statusRegister, STATUS_REGISTER_V);
        case 12: // GT
            return get_not_bit_bool(statusRegister, STATUS_REGISTER_Z) && (get_not_bit_bool(statusRegister, STATUS_REGISTER_N) == get_not_bit_bool(statusRegister, STATUS_REGISTER_V));
        case 13: // LE
            return get_bit_bool(statusRegister, STATUS_REGISTER_Z) && (get_not_bit_bool(statusRegister, STATUS_REGISTER_N) != get_not_bit_bool(statusRegister, STATUS_REGISTER_V));
        case 14: // AL
            return true;
        case 15: // NV
            return true;
        }

        return false;
    }

    const u32 cpu::shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift, u32& _carryFlag)
    {
        u32 operand = _operand;
        u32 shift = _zeroShift ? 32 : _shift;

        switch (_shiftType)
            {
            case 0x0: // LSL
                if (_zeroShift)
                    break;
                _carryFlag = (operand << shift - 1) >> 31;
                operand <<= shift;
                break;
            case 0x1: // LSR
                _carryFlag = (operand >> shift - 1) & 0b1;
                operand >>= shift;
                break;
            case 0x2: // ASR
                // compiler must be able to perform asr for this to work correctly!!
                _carryFlag = ((s32)operand >> shift - 1) & 0b1;
                operand = (s32)operand >> shift; 
                break;
            case 0x3: // ROR
                _carryFlag = operand & 0b1;
                if (_zeroShift) // RCR
                {
                    operand >>= 1;
                    operand |= (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C) << 31;
                }
                else
                {
                    operand = rotate_right(operand, shift);
                }
                break;
            }

        return operand;
    }

    const u32 cpu::shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift)
    {
        u32 tempCarry = 0;
        return shift_operand(_shiftType, _zeroShift, _operand, _shift, tempCarry);
    }

    const u32 cpu::arm_dataproc(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isImmediate = get_bit_bool(_opcode, 1 << 25);
        bool setStatus = get_bit_bool(_opcode, 1 << 20);

        u32 dataOpcode = (_opcode >> 21) & 0b1111;
        u32 regN = get_register((_opcode >> 16) & 0b1111);
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
            bool shiftRegister = get_bit_bool(_opcode, 1 << 4);
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
            }

            operand = get_register(_opcode & 0b1111);
            operand = shift_operand(shiftType, zeroShift, operand, shift, carry);

            if (zeroShiftRegister)
                carry = 0;
        }

        bool setRegister = false;
        bool isLogical = false;
        u32 result = 0;
        u32 overflow = 0;
        switch (dataOpcode)
        {
        case 0x0: // AND
            result = regN & operand;
            setRegister = true;
            isLogical = true;
            break;
        case 0x1: // EOR
            result = regN ^ operand;
            setRegister = true;
            isLogical = true;
            break;
        case 0x2: // SUB
            result = regN - operand;
            overflow = test_overflow_neg(regN, operand);
            carry = test_carry_neg(regN, operand);
            setRegister = true;
            break;
        case 0x3: // RSB
            result = operand - regN;
            overflow = test_overflow_neg(operand, regN);
            carry = test_carry_neg(operand, regN);
            setRegister = true;
            break;
        case 0x4: // ADD
            result = regN + operand;
            overflow = test_overflow_pos(regN, operand);
            carry = test_carry_pos(regN, operand);
            setRegister = true;
            break;
        case 0x5: // ADC
            result = regN + operand + (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C);
            overflow = test_overflow_pos(regN, operand, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            carry = test_carry_pos(regN, operand, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            setRegister = true;
            break;
        case 0x6: // SBC
            result = regN - operand + (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C) - 1;
            overflow = test_overflow_neg(regN, operand, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            carry = test_carry_neg(regN, operand, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            setRegister = true;
            break;
        case 0x7: // RSC
            result = operand - regN + (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C) - 1;
            overflow = test_overflow_neg(operand, regN, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            carry = test_carry_neg(operand, regN, (u32)get_bit_bool(statusRegister, STATUS_REGISTER_C));
            setRegister = true;
            break;
        case 0x8: // TST
            result = regN & operand;
            isLogical = true;
            break;
        case 0x9: // TEQ
            result = regN ^ operand;
            isLogical = true;
            break;
        case 0xA: // CMP
            result = regN - operand;
            overflow = test_overflow_neg(regN, operand);
            carry = test_carry_neg(regN, operand);
            isLogical = true;
            break;
        case 0xB: // CMN
            result = regN + operand;
            overflow = test_overflow_pos(regN, operand);
            carry = test_carry_pos(regN, operand);
            isLogical = true;
            break;
        case 0xC: // ORR
            result = regN | operand;
            setRegister = true;
            isLogical = true;
            break;
        case 0xD: // MOV
            result = operand;
            setRegister = true;
            isLogical = true;
            break;
        case 0xE: // BIC
            result = regN & ~operand;
            setRegister = true;
            isLogical = true;
            break;
        case 0xF: // MVN
            result = ~operand;
            setRegister = true;
            isLogical = true;
            break;
        }

        if (setRegister)
            regD = result;

        if (setStatus)
        {
            if (!isLogical)
                set_bit(statusRegister, STATUS_REGISTER_V_SHIFT, overflow);
            set_bit(statusRegister, STATUS_REGISTER_C_SHIFT, carry);
            set_bit(statusRegister, STATUS_REGISTER_Z_SHIFT, result == 0);
            set_bit(statusRegister, STATUS_REGISTER_N_SHIFT, result >> STATUS_REGISTER_N_SHIFT);
        }

        return 0;
    }

    const u32 cpu::arm_branch(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool setLink = get_bit_bool(_opcode, 1 << 24);
        s32 offset = ((s32)_opcode << 8) >> 6;

        programCounter += ARM_WORD_LENGTH + offset;

        if (setLink)
            get_register(REGISTER_LINK_INDEX) = programCounter;

        return 0;
    }

    const u32 cpu::arm_branch_ex(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        u32 branchType = (_opcode >> 4) & 0b1111;
        u32 regN = get_register(_opcode & 0b1111);

        // ARMv4 only supports BX (switch to THUMB mode)
        if (branchType == 0b0001)
        {
            bool thumbMode = regN & 0b1;
            
            set_bit(statusRegister, STATUS_REGISTER_T_SHIFT, thumbMode);
            
            if (thumbMode)
                programCounter = regN | 0b1;
        }

        return 0;
    }

    const u32 cpu::arm_trans_single(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isImmediate = get_not_bit_bool(_opcode, 1 << 25);
        bool isPreOffset = get_bit_bool(_opcode, 1 << 24);
        bool isByteTransfer = get_bit_bool(_opcode, 1 << 22);
        bool isLoad = get_bit_bool(_opcode, 1 << 20);

        u32 offsetSign = get_bit_bool(_opcode, 1 << 23);
        u32& regD = get_register((_opcode >> 12) & 0b1111);
        u32& regN = get_register((_opcode >> 16) & 0b1111);

        u32 offset = 0;
        if (isImmediate)
        {
            offset = _opcode & 0xFFF;
        }
        else
        {
            u32 regOffset = get_register(_opcode & 0b1111);
            u32 shiftType = (_opcode >> 5) & 0b11;
            u32 shift = (_opcode >> 7) & 0b11111;
            
            offset = shift_operand(shiftType, shift == 0, regOffset, shift);
        }

        u32 destAddress = regN;
        if (isPreOffset)
        {
            destAddress = sub_or_add(destAddress, offset, offsetSign);
            
            bool writeBack = get_bit_bool(_opcode, 1 << 21);
            if (writeBack)
                regN = destAddress;
        }

        if (isLoad)
        {
            u32 data = addressBus.read_32(destAddress);
            regD = isByteTransfer ? data & 0xFF : data;
        }
        else
        {
            if (isByteTransfer)
            {
                addressBus.write_8(destAddress, regD & 0xFF);
            }
            else
            {
                addressBus.write_32(destAddress, regD);
            }
        }

        // post offset, writeback always enabled
        if (!isPreOffset)
        {
            destAddress = sub_or_add(destAddress, offset, offsetSign);
            regN = destAddress;
        }

        return 0;
    }

    const u32 cpu::arm_trans_half(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isPreOffset = get_bit_bool(_opcode, 1 << 24);
        bool isImmediate = get_bit_bool(_opcode, 1 << 22);
        bool isLoad = get_bit_bool(_opcode, 1 << 20);

        u32 offsetSign = get_bit_bool(_opcode, 1 << 23);
        u32& regD = get_register((_opcode >> 12) & 0b1111);
        u32& regN = get_register((_opcode >> 16) & 0b1111);
        
        u32 offset = 0;
        if (isImmediate)
        {
            offset = ((_opcode >> 4) & 0xF0) | (_opcode & 0b1111);
        }
        else
        {
            offset = get_register(_opcode & 0b1111);
        }

        u32 destAddress = regN;
        if (isPreOffset)
        {
            destAddress = sub_or_add(destAddress, offset, offsetSign);
            
            bool writeBack = get_bit_bool(_opcode, 1 << 21);
            if (writeBack)
                regN = destAddress;
        }

        u32 transType = (_opcode >> 5) & 0b11;
        if (isLoad)
        {
            u32 data = 0;
            switch (transType)
            {
            case 0b01: // LDRH
                data = addressBus.read_16(destAddress);
                break;
            case 0b10: // LDRSB
                data = addressBus.read_8(destAddress);
                data |= 0xFFFFFF00 * (data >> 7);
                break;
            case 0b11: // LDRSH
                data = addressBus.read_16(destAddress);
                data |= 0xFFFF0000 * (data >> 15);
                break;                
            }

            regD = data;
        }
        else
        {
            // only STRH for ARMv4
            if (transType == 0b01)
                addressBus.write_16(destAddress, regN & 0xFFFF);
        }

        if (!isPreOffset)
        {
            destAddress = sub_or_add(destAddress, offset, offsetSign);
            regN = destAddress;
        }

        return 0;
    }

    const u32 cpu::arm_trans_swap(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isByteTransfer = get_bit_bool(_opcode, 1 << 22);

        u32& regN = get_register((_opcode >> 16) & 0b1111);
        u32& regD = get_register((_opcode >> 12) & 0b1111);
        u32& regM = get_register(_opcode & 0b1111);

        if (isByteTransfer)
        {
            regD = addressBus.read_8(regN);
            addressBus.write_8(regN, regM & 0xFF);
        }
        else
        {
            regD = addressBus.read_32(regN);
            addressBus.write_32(regN, regM);
        }

        return 0;
    }

    const u32 cpu::arm_trans_block(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isPreOffset = get_bit_bool(_opcode, 1 << 24);
        bool isUserMode = get_bit_bool(_opcode, 1 << 22);
        bool writeBack = get_bit_bool(_opcode, 1 << 21);
        bool isLoad = get_bit_bool(_opcode, 1 << 20);

        u32& regN = get_register((_opcode >> 16) & 0b1111);
        u32 regList = _opcode & 0xFFFF;

        u32 offsetSign = get_bit_bool(_opcode, 1 << 23);
        u32 offset = bit_count(regList, REGISTER_LIST_LENGTH) * ARM_WORD_LENGTH;
        
        u32 destAddress = regN - bool_lerp(offset, 0, offsetSign);
        for (u32 i = 0; i < REGISTER_LIST_LENGTH; ++i)
        {
            bool useRegister = (regList >> i) & 0b1;

            if (useRegister)
            {
                destAddress += bool_lerp(0, ARM_WORD_LENGTH, isPreOffset);

                u32& regData = get_register(i);
                if (isLoad)
                {
                    regData = addressBus.read_32(destAddress);
                }
                else
                {
                    addressBus.write_32(destAddress, regData);
                }

                destAddress += bool_lerp(ARM_WORD_LENGTH, 0, isPreOffset);
            }
        }

        if (writeBack)
            regN = sub_or_add(regN, offset, offsetSign);;

        return 0;
    }

    const u32 cpu::arm_multiply(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool setStatus = get_bit_bool(_opcode, 1 << 20);

        u32& regHi = get_register((_opcode >> 16) & 0b1111);
        u32& regLo = get_register((_opcode >> 12) & 0b1111);
        u64 regHiLo = ((u64)regHi << ARM_WORD_BIT_LENGTH) | regLo;
        u64 regS = get_register((_opcode >> 8) & 0b1111);
        u64 regM = get_register(_opcode & 0b1111);

        bool setRegLo = false;
        u64 result = 0;
        u32 multiplyType = (_opcode >> 21) & 0b1111;
        switch (multiplyType)
        {
        case 0b0000: // MUL
            result = regM * regS;
            break;
        case 0b0001: // MLA
            result = regM * regS + regLo;
            break;
        case 0b0100: // UMULL
            result = regM * regS;
            setRegLo = true;
            break;
        case 0b0101: // UMLAL
            result = regM * regS + regHiLo;
            setRegLo = true;
            break;
        case 0b0110: // SMULL
            result = (s64)regM * (s64)regS;
            setRegLo = true;
            break;
        case 0b0111: // SMLAL
            result = (s64)regM * (s64)regS + (s64)regHiLo;
            setRegLo = true;
            break;
        }

        if (setRegLo)
        {
            regHi = result >> ARM_WORD_BIT_LENGTH;
            regLo = result & 0xFFFFFFFF;
        }
        else
        {
            regHi = result & 0xFFFFFFFF;
            result <<= ARM_WORD_BIT_LENGTH;
        }

        if (setStatus)
        {
            if (setRegLo)
                set_bit(statusRegister, STATUS_REGISTER_V_SHIFT, 0);
            set_bit(statusRegister, STATUS_REGISTER_C_SHIFT, 0);
            set_bit(statusRegister, STATUS_REGISTER_Z_SHIFT, result == 0);
            set_bit(statusRegister, STATUS_REGISTER_N_SHIFT, result >> 63);
        }

        return 0;
    }

    const u32 cpu::arm_psr(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isImmediate = get_bit_bool(_opcode, 1 << 25);
        bool useSPSR = get_bit_bool(_opcode, 1 << 22);
        bool setPSR = get_bit_bool(_opcode, 1 << 21);

        bool isUserMode = false;
        u32& currentSPSR = get_current_spsr(isUserMode);
        if (useSPSR && isUserMode)
            return 0;

        u32& regD = get_register(_opcode & 0b1111);
        u32 operand = 0;
        if (isImmediate)
        {
            u32 shift = ((_opcode >> 8) & 0b1111) * 2;
            operand = _opcode & 0xFF;
            operand = rotate_right(operand, shift);
        }
        else
        {
            operand = regD;
        }

        u32 tempPSR = useSPSR ? currentSPSR : statusRegister;
        if (setPSR)
        {
            bool setFlags = get_bit_bool(_opcode, 1 << 19);
            bool setControl = get_bit_bool(_opcode, 1 << 16);
            u32 preserveMask = tempPSR & (STATUS_REGISTER_T | STATUS_PRESERVE_MASK);

            tempPSR = ((STATUS_FLAGS_MASK & operand) * setFlags)
                    | ((STATUS_CONTROL_MASK & operand) * setControl)
                    | preserveMask;
            
            (useSPSR ? currentSPSR : statusRegister) = tempPSR;
        }
        else
        {
            regD = tempPSR;
        }

        return 0;
    }

    void cpu::create_arm_isa()
    {
        armISA[0] = { ARM_DATAPROC_1_MASK, ARM_DATAPROC_1_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),         "Data Proc 1" };
        armISA[1] = { ARM_DATAPROC_2_MASK, ARM_DATAPROC_2_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),         "Data Proc 2" };
        armISA[2] = { ARM_DATAPROC_3_MASK, ARM_DATAPROC_3_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),         "Data Proc 3" };
        armISA[3] = { ARM_MULTIPLY_1_MASK, ARM_MULTIPLY_1_TEST, std::bind(&cpu::arm_multiply, this, std::placeholders::_1),         "Multiply 1" };
        armISA[4] = { ARM_MULTIPLY_2_MASK, ARM_MULTIPLY_2_TEST, std::bind(&cpu::arm_multiply, this, std::placeholders::_1),         "Multiply 2" };
        armISA[5] = { ARM_BRANCHING_1_MASK, ARM_BRANCHING_1_TEST, std::bind(&cpu::arm_branch_ex, this, std::placeholders::_1),      "Branch Ex" };
        armISA[6] = { ARM_BRANCHING_2_MASK, ARM_BRANCHING_2_TEST, std::bind(&cpu::arm_branch, this, std::placeholders::_1),         "Branch" };
        armISA[7] = { ARM_TRANSFER_1_MASK, ARM_TRANSFER_1_TEST, std::bind(&cpu::arm_trans_single, this, std::placeholders::_1),     "Transfer Single 1" };
        armISA[8] = { ARM_TRANSFER_2_MASK, ARM_TRANSFER_2_TEST, std::bind(&cpu::arm_trans_single, this, std::placeholders::_1),     "Transfer Single 2" };
        armISA[9] = { ARM_TRANSFER_3_MASK, ARM_TRANSFER_3_TEST, std::bind(&cpu::arm_trans_half, this, std::placeholders::_1),       "Transfer Half 1" };
        armISA[10] = { ARM_TRANSFER_4_MASK, ARM_TRANSFER_4_TEST, std::bind(&cpu::arm_trans_half, this, std::placeholders::_1),      "Transfer Half 2" };
        armISA[11] = { ARM_TRANSFER_5_MASK, ARM_TRANSFER_5_TEST, std::bind(&cpu::arm_trans_swap, this, std::placeholders::_1),      "Transfer Swap" };
        armISA[12] = { ARM_TRANSFER_6_MASK, ARM_TRANSFER_6_TEST, std::bind(&cpu::arm_trans_block, this, std::placeholders::_1),     "Transfer Block" };

        sort_isa_array<cpu_instruction, ARM_ISA_COUNT, ARM_WORD_BIT_LENGTH>(armISA);
    }

    void cpu::reset_registers()
    {
        for (u32 i = 0; i < 8; ++i)
            thumbRegisters[i] = 0;

        for (u32 i = 0; i < 10; ++i)
            armRegisters[i] = 0;

        for (u32 i = 0; i < 6; ++i)
        {
            stackPointers[i] = 0;
            linkRegisters[i] = 0;
        }

        for (u32 i = 0; i < 5; ++i)
            savedStatusRegisters[i] = 0;

        statusRegister = 0;
        programCounter = 0;
    }

    cpu::cpu(bus& _addressBus)
        : addressBus{ _addressBus },
        bankedRegisterOffset{ 0 }, armRegisterOffset{ 0 }
    {
        reset_registers();
        create_arm_isa();
    }
}