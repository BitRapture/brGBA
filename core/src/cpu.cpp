#include "../include/cpu.h"

namespace br::gba
{
    void cpu::cycle()
    {
        if (cycleStallCount > 0)
        {
            --cycleStallCount;
            return;
        }
        if (cycleNonSeqStallCount > 0)
        {
            --cycleNonSeqStallCount;
            return;
        }
        if (cycleSeqStallCount > 0)
        {
            --cycleSeqStallCount;
            return;
        }

        if (isArmMode)
            decode_arm_instruction();
        else
            decode_thumb_instruction();
    }

    void cpu::decode_arm_instruction()
    {

    }

    void cpu::decode_thumb_instruction()
    {

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
}