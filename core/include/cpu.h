#pragma once
#include "typedefs.h"

namespace br::gba
{
    class bus;

    class cpu
    {
    public:
        /// @brief Step the cpu
        void cycle();

    private:
        /// @brief decode 32-bit arm instruction
        void decode_arm_instruction();
        /// @brief decode 16-bit thumb instruction
        void decode_thumb_instruction();   

        /// @brief get registers 0 - 15
        /// @param _index register index
        /// @return register by reference
        u32& get_register(const u32& _index);

        const bool check_condition(const u32& _code);

    private:
        // general purpose registers 0 - 7
        u32 thumbRegisters[8];
        // general purpose registers 8 - 12
        // fiq banked registers 8 - 12
        u32 armRegisters[10];
        // stack pointer 13
        // fiq, svc, abt, irq, und banked stack pointer 13
        u32 stackPointers[6];
        // link register 14
        // fiq, svc, abt, irq, und banked link register 14
        u32 linkRegisters[6];
        // saved status register for operation modes
        u32 savedStatusRegisters[5];
        // program counter
        u32 programCounter;
        // current program status register
        u32 statusRegister;

    private:
        // either 0, or 5 for fiq banked registers
        u32 armRegisterOffset;
        // used for link and stack pointer banked registers
        u32 bankedRegisterOffset;

    private:
        // cpu state, either arm or thumb
        bool isArmMode;

    private:
        bus& addressBus;

    public:
        cpu(bus& _addressBus);
    };
}