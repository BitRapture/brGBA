#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/constants.h"

namespace br::gba
{
    void cpu::cycle()
    {
        if (isArmMode)
            decode_arm_instruction();
        else
            decode_thumb_instruction();
    }

    void cpu::decode_arm_instruction()
    {
        u32 opcode = addressBus.read_32(programCounter);
        programCounter += 4;

        u32 condition = condition_mask(opcode);
        if (!check_condition(condition))
            return;
        
        
    }

    void cpu::decode_thumb_instruction()
    {
        u16 opcode = addressBus.read_16(programCounter);
        programCounter += 2;
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
            return get_bit(statusRegister, STATUS_REGISTER_N) == get_bit(statusRegister, STATUS_REGISTER_V);
        case 11: // LT
            return (get_bit(statusRegister, STATUS_REGISTER_N) < get_bit(statusRegister, STATUS_REGISTER_V)) || 
                (get_bit(statusRegister, STATUS_REGISTER_N) > get_bit(statusRegister, STATUS_REGISTER_V));
        case 12: // GT
            return get_not_bit(statusRegister, STATUS_REGISTER_Z) && (get_bit(statusRegister, STATUS_REGISTER_N) == get_bit(statusRegister, STATUS_REGISTER_V));
        case 13: // LE
            return get_bit(statusRegister, STATUS_REGISTER_Z) && ((get_bit(statusRegister, STATUS_REGISTER_N) < get_bit(statusRegister, STATUS_REGISTER_V)) || 
                (get_bit(statusRegister, STATUS_REGISTER_N) > get_bit(statusRegister, STATUS_REGISTER_V)));
        case 14: // AL
            return true;
        case 15: // NV
            return false;
        }
    }

    cpu::cpu(bus& _addressBus)
        : addressBus{ _addressBus },
        bankedRegisterOffset{ 0 }, armRegisterOffset{ 0 },
        isArmMode{ true }
    {

    }
}