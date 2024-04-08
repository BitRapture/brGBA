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

    void cpu::reset()
    {
        trigger_exception(cpu_exception::RESET);
    }

    void cpu::interrupt()
    {
        if (get_bit_bool(statusRegister, STATUS_REGISTER_I))
            return;

        trigger_exception(cpu_exception::IRQ);
    }

    void cpu::fast_interrupt()
    {
        if (get_bit_bool(statusRegister, STATUS_REGISTER_F))
            return;

        trigger_exception(cpu_exception::FIQ);
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
        statusInfo << debug_print_status_registers();

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

    void cpu::debug_log_cycle(const u32& _opcode, const cpu_instruction& _instruction)
    {
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
        u32 isaSize = _armISA ? ARM_ISA_COUNT : THUMB_ISA_COUNT;
        std::string isaList;

        for (u32 i = 0; i < isaSize; ++i)
        {
            std::string debugInfo = (_armISA ? armISA[i] : thumbISA[i]).debug_info;
            std::string testMask = bit_string((_armISA ? armISA[i] : thumbISA[i]).data_test, true, false);
            isaList += "i: " + std::to_string(i) + ", " + debugInfo + ", " + testMask + "\n";
        }
         
        return isaList;
    }

    const std::string cpu::debug_print_status_registers()
    {
        std::array<char, ARM_WORD_BIT_LENGTH> statusSymbols;
        statusSymbols.fill('-');
        statusSymbols[STATUS_REGISTER_N_SHIFT] = 'N';
        statusSymbols[STATUS_REGISTER_Z_SHIFT] = 'Z';
        statusSymbols[STATUS_REGISTER_C_SHIFT] = 'C';
        statusSymbols[STATUS_REGISTER_V_SHIFT] = 'V';
        statusSymbols[STATUS_REGISTER_I_SHIFT] = 'I';
        statusSymbols[STATUS_REGISTER_F_SHIFT] = 'F';
        statusSymbols[STATUS_REGISTER_T_SHIFT] = 'T';
        statusSymbols[0] = '1';
        statusSymbols[1] = '1';
        statusSymbols[2] = '1';
        statusSymbols[3] = '1';
        statusSymbols[4] = '1';

        std::array<std::string, 6> statusRegisters;
        statusRegisters[(u32)cpu_mode::FIQ] = "FIQ SPSR: [";
        statusRegisters[(u32)cpu_mode::IRQ] = "IRQ SPSR: [";
        statusRegisters[(u32)cpu_mode::SUPERVISOR] = "SVC SPSR: [";
        statusRegisters[(u32)cpu_mode::ABORT] = "ABT SPSR: [";
        statusRegisters[(u32)cpu_mode::UNDEFINED] = "UND SPSR: [";
        statusRegisters[5] = "CPSR: [";
        for (u32 i = 0; i < ARM_WORD_BIT_LENGTH; ++i)
        {
            u32 x = ARM_WORD_BIT_LENGTH - 1 - i;
            char symbol = statusSymbols[x];

            if ((x <= 26 && x >= 8))
                continue;

            statusRegisters[(u32)cpu_mode::FIQ] += ((savedStatusRegisters[(u32)cpu_mode::FIQ] >> x) & 0b1) ? symbol : '-';
            statusRegisters[(u32)cpu_mode::IRQ] += ((savedStatusRegisters[(u32)cpu_mode::IRQ] >> x) & 0b1) ? symbol : '-';
            statusRegisters[(u32)cpu_mode::SUPERVISOR] += ((savedStatusRegisters[(u32)cpu_mode::SUPERVISOR] >> x) & 0b1) ? symbol : '-';
            statusRegisters[(u32)cpu_mode::ABORT] += ((savedStatusRegisters[(u32)cpu_mode::ABORT] >> x) & 0b1) ? symbol : '-';
            statusRegisters[(u32)cpu_mode::UNDEFINED] += ((savedStatusRegisters[(u32)cpu_mode::UNDEFINED] >> x) & 0b1) ? symbol : '-';
            statusRegisters[5] += ((statusRegister >> x) & 0b1) ? symbol : '-';
        }

        std::string registers;
        for (u32 i = 0; i < statusRegisters.size(); ++i)
        {
            registers += statusRegisters[i] + "]\n";
        }

        return registers;
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
                debug_log_cycle(opcode, currentInstruction);
                return cycleCount;
            }
        }

        debug_log_cycle(opcode, {});

        return 0;
    }

    const u32 cpu::decode_thumb_instruction()
    {
        u16 opcode = addressBus.read_16(programCounter);
        programCounter += THUMB_WORD_LENGTH;

        for (u32 i = 0; i < THUMB_ISA_COUNT; ++i)
        {
            cpu_instruction currentInstruction = thumbISA[i];

            if ((currentInstruction.data_mask & opcode) == currentInstruction.data_test)
            {
                u32 cycleCount = 0;
                cycleCount = currentInstruction.execute(opcode);
                debug_log_cycle(opcode, currentInstruction);
                return cycleCount;
            }
        }

        debug_log_cycle(opcode, {});

        return 0;
    }

    u32& cpu::get_register(const u32& _index, const bool& _forceUser)
    {
        cpu_mode mode = get_current_mode();
        bool is_not_user = mode != cpu_mode::USER && mode != cpu_mode::SYSTEM && !_forceUser;
        bool is_fiq = mode == cpu_mode::FIQ && !_forceUser;
    
        u32 armRegisterOffset = REGISTER_ARM_OFFSET * is_fiq;
        u32 bankedRegisterOffset = ((u32)mode + 1) * is_not_user;
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
        case 0b10011: // SUPERVISOR
            return cpu_mode::SUPERVISOR;
        case 0b10111: // Abort
            return cpu_mode::ABORT;
        case 0b11011: // Undefined
            return cpu_mode::UNDEFINED;
        case 0b11111: // Privelleged User
            return cpu_mode::SYSTEM;
        }

        return cpu_mode::UNDEFINED;
    }

    void cpu::set_current_mode(const cpu_mode& _mode)
    {
        u32 modeStatus = 0;
        switch (_mode)
        {
        case cpu_mode::USER:
            modeStatus = 0b10000;
            break;
        case cpu_mode::FIQ:
            modeStatus = 0b10001;
            break;
        case cpu_mode::IRQ:
            modeStatus = 0b10010;
            break;
        case cpu_mode::SUPERVISOR:
            modeStatus = 0b10011;
            break;
        case cpu_mode::ABORT:
            modeStatus = 0b10111;
            break;
        case cpu_mode::UNDEFINED:
            modeStatus = 0b11011;
            break;
        case cpu_mode::SYSTEM:
            modeStatus = 0b11111;
            break;
        }

        statusRegister = (statusRegister & 0xFFFFFFE0) | modeStatus;
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
                if (_zeroShift && !get_bit_bool(statusRegister, STATUS_REGISTER_T)) // RCR
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

    void cpu::trigger_exception(const cpu_exception& _exception)
    {
        u32 previousPSR = statusRegister;
        u32 exceptionVector = 0;
        bool disableFIQ = false;
        switch (_exception)
        {
        case cpu_exception::RESET:
            exceptionVector = EXCEPTION_ADDR_RESET;
            set_current_mode(cpu_mode::SUPERVISOR);
            disableFIQ = true;
            break;
        case cpu_exception::SWI:
            exceptionVector = EXCEPTION_ADDR_SWI;
            set_current_mode(cpu_mode::SUPERVISOR);
            break;
        case cpu_exception::PREFETCH_ABORT:
            exceptionVector = EXCEPTION_ADDR_PREFETCH;
            set_current_mode(cpu_mode::ABORT);
            break;
        case cpu_exception::FIQ:
            exceptionVector = EXCEPTION_ADDR_FIQ;
            set_current_mode(cpu_mode::FIQ);
            disableFIQ = true;
            break;
        case cpu_exception::IRQ:
            exceptionVector = EXCEPTION_ADDR_IRQ;
            set_current_mode(cpu_mode::IRQ);
            break;
        case cpu_exception::UNDEFINED:
            exceptionVector = EXCEPTION_ADDR_UNDEFINED;
            set_current_mode(cpu_mode::UNDEFINED);
            break;
        }

        get_register(REGISTER_LINK_INDEX) = programCounter;
        
        bool isUserMode;
        get_current_spsr(isUserMode) = previousPSR;

        set_bit(statusRegister, STATUS_REGISTER_T_SHIFT, 0);
        set_bit(statusRegister, STATUS_REGISTER_I_SHIFT, 1);
        if (disableFIQ)
            set_bit(statusRegister, STATUS_REGISTER_F_SHIFT, 1);

        programCounter = exceptionVector;
    }

    const u32 cpu::arm_dataproc(const u32& _opcode)
    {
        if (!check_condition(_opcode >> ARM_CONDITION_SHIFT))
            return 0;

        bool isImmediate = get_bit_bool(_opcode, 1 << 25);
        bool setStatus = get_bit_bool(_opcode, 1 << 20);

        u32 dataOpcode = (_opcode >> 21) & 0b1111;
        u32 regDIndex = (_opcode >> 12) & 0b1111;
        bool isProgramCounter = regDIndex == REGISTER_PROGRAM_COUNTER_INDEX;

        u32 regN = get_register((_opcode >> 16) & 0b1111);
        u32& regD = get_register(regDIndex);

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
            if (isProgramCounter)
            {
                bool isUserMode;
                statusRegister = get_current_spsr(isUserMode);
            }
            else
            {
                if (!isLogical)
                    set_bit(statusRegister, STATUS_REGISTER_V_SHIFT, overflow);
                set_bit(statusRegister, STATUS_REGISTER_C_SHIFT, carry);
                set_bit(statusRegister, STATUS_REGISTER_Z_SHIFT, result == 0);
                set_bit(statusRegister, STATUS_REGISTER_N_SHIFT, result >> STATUS_REGISTER_N_SHIFT);
            }
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
            
            programCounter = regN + ARM_WORD_LENGTH - 0b1;
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

        bool containsPC = get_bit_bool(_opcode, 1 << 15);
        bool isLoad = get_bit_bool(_opcode, 1 << 20);
        bool isUserMode = get_bit_bool(_opcode, 1 << 22);
        bool isPreOffset = get_bit_bool(_opcode, 1 << 24);
        bool isModeChange = isLoad && containsPC && isUserMode;
        bool useUserMode = isUserMode && !isModeChange;
        bool writeBack = get_bit_bool(_opcode, 1 << 21) && !useUserMode;

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

                u32& regData = get_register(i, useUserMode);
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
            regN = sub_or_add(regN, offset, offsetSign);

        if (isModeChange)
        {
            bool userMode;
            statusRegister = get_current_spsr(userMode);
        }

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

        bool isUserMode;
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
            bool setControl = get_bit_bool(_opcode, 1 << 16) && get_current_mode() != cpu_mode::USER;
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

    const u32 cpu::arm_soft_interrupt(const u32& _opcode)
    {
        trigger_exception(cpu_exception::SWI);

        return 0;
    }

    const u32 cpu::thumb_shift(const u32& _opcode)
    {
        const u32 conditionAlways = 0xE << 28;
        const u32 moveOp = 0xD << 21;
        const u32 setStatus = 1 << 20;

        u32 offset = (_opcode << 1) & (0b11111 << 7);
        u32 shiftOp = (_opcode >> 6) & (0b11 << 5);
        u32 regS = (_opcode >> 3) & 0b111;
        u32 regD = (_opcode & 0b111) << 12;

        u32 opcode = conditionAlways | moveOp | setStatus | regD | offset | shiftOp | regS;
        arm_dataproc(opcode);

        return 0;
    }

    const u32 cpu::thumb_data_reg(const u32& _opcode)
    {
        const u32 conditionAlways = 0xE << 28;
        const u32 setStatus = 1 << 20;
        
        u32 isImmediate = (_opcode << 15) & (1 << 25);
        u32 dataOp = (4 >> ((_opcode >> 9) & 0b1)) << 21;
        u32 operand = (_opcode >> 6) & 0b111;
        u32 regS = (_opcode << 13) & (0b111 << 16);
        u32 regD = (_opcode & 0b111) << 12;

        u32 opcode = conditionAlways | isImmediate | dataOp | setStatus | regS | regD | operand;
        arm_dataproc(opcode);

        return 0;
    }

    void cpu::create_arm_isa()
    {
        armISA[0] = { ARM_DATAPROC_1_MASK, ARM_DATAPROC_1_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),                     "ARM Data Proc 1" };
        armISA[1] = { ARM_DATAPROC_2_MASK, ARM_DATAPROC_2_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),                     "ARM Data Proc 2" };
        armISA[2] = { ARM_DATAPROC_3_MASK, ARM_DATAPROC_3_TEST, std::bind(&cpu::arm_dataproc, this, std::placeholders::_1),                     "ARM Data Proc 3" };
        armISA[3] = { ARM_MULTIPLY_1_MASK, ARM_MULTIPLY_1_TEST, std::bind(&cpu::arm_multiply, this, std::placeholders::_1),                     "ARM Multiply 1" };
        armISA[4] = { ARM_MULTIPLY_2_MASK, ARM_MULTIPLY_2_TEST, std::bind(&cpu::arm_multiply, this, std::placeholders::_1),                     "ARM Multiply 2" };
        armISA[5] = { ARM_BRANCHING_1_MASK, ARM_BRANCHING_1_TEST, std::bind(&cpu::arm_branch_ex, this, std::placeholders::_1),                  "ARM Branch Ex" };
        armISA[6] = { ARM_BRANCHING_2_MASK, ARM_BRANCHING_2_TEST, std::bind(&cpu::arm_branch, this, std::placeholders::_1),                     "ARM Branch" };
        armISA[7] = { ARM_TRANSFER_1_MASK, ARM_TRANSFER_1_TEST, std::bind(&cpu::arm_trans_single, this, std::placeholders::_1),                 "ARM Transfer Single 1" };
        armISA[8] = { ARM_TRANSFER_2_MASK, ARM_TRANSFER_2_TEST, std::bind(&cpu::arm_trans_single, this, std::placeholders::_1),                 "ARM Transfer Single 2" };
        armISA[9] = { ARM_TRANSFER_3_MASK, ARM_TRANSFER_3_TEST, std::bind(&cpu::arm_trans_half, this, std::placeholders::_1),                   "ARM Transfer Half 1" };
        armISA[10] = { ARM_TRANSFER_4_MASK, ARM_TRANSFER_4_TEST, std::bind(&cpu::arm_trans_half, this, std::placeholders::_1),                  "ARM Transfer Half 2" };
        armISA[11] = { ARM_TRANSFER_5_MASK, ARM_TRANSFER_5_TEST, std::bind(&cpu::arm_trans_swap, this, std::placeholders::_1),                  "ARM Transfer Swap" };
        armISA[12] = { ARM_TRANSFER_6_MASK, ARM_TRANSFER_6_TEST, std::bind(&cpu::arm_trans_block, this, std::placeholders::_1),                 "ARM Transfer Block" };
        armISA[13] = { ARM_STATUSTRANS_1_MASK, ARM_STATUSTRANS_1_TEST, std::bind(&cpu::arm_psr, this, std::placeholders::_1),                   "ARM Status Transfer 1" };
        armISA[14] = { ARM_STATUSTRANS_2_MASK, ARM_STATUSTRANS_2_TEST, std::bind(&cpu::arm_psr, this, std::placeholders::_1),                   "ARM Status Transfer 2" };
        armISA[15] = { ARM_SOFTINTERRUPT_MASK, ARM_SOFTINTERRUPT_TEST, std::bind(&cpu::arm_soft_interrupt, this, std::placeholders::_1),        "ARM Software Interrupt" };

        sort_isa_array<cpu_instruction, ARM_ISA_COUNT, ARM_WORD_BIT_LENGTH>(armISA);
    }

    void cpu::create_thumb_isa()
    {
        thumbISA[0] = { THUMB_SHIFT_MASK, THUMB_SHIFT_TEST, std::bind(&cpu::thumb_shift, this, std::placeholders::_1),                           "THUMB Shift" };
        thumbISA[1] = { THUMB_DATA_REG_MASK, THUMB_DATA_REG_TEST, std::bind(&cpu::thumb_data_reg, this, std::placeholders::_1),                  "THUMB Data Proc Reg (ADD/SUB)" };
    
        sort_isa_array<cpu_instruction, THUMB_ISA_COUNT, THUMB_WORD_BIT_LENGTH>(thumbISA);
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
        : addressBus{ _addressBus }
    {
        reset_registers();
        create_arm_isa();
        create_thumb_isa();
    }
}