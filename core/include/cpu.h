#pragma once
#include "typedefs.h"
#include "constants.h"
#include <functional>
#include <array>
#include <string>

namespace br::gba
{
    class bus;

    struct cpu_instruction
    {
        u32 data_mask;
        u32 data_test;
        std::function<const u32(const u32&)> execute;
    };

    class cpu
    {
    public:
        /// @brief step the cpu
        /// @return cycle count
        const u32 cycle();

        /// @brief cycle cpu with an instruction, for debug purposes
        /// @param _instruction instruction to test
        /// @return cycle count
        const u32 debug_cycle_instruction(const u32& _instruction);

        /// @brief print status information of the cpu, for debug purposes
        /// @return formatted status information
        const std::string debug_print_status();

        void debug_save_log(const std::string& _filePath);

        void debug_log_arm_cycle(const u32& _opcode, const cpu_instruction& _instruction);

    private:
        /// @brief decode 32-bit arm instruction
        /// @return cycle count
        const u32 decode_arm_instruction();
        /// @brief decode 16-bit thumb instruction
        /// @return cycle count
        const u32 decode_thumb_instruction();   

        /// @brief get registers 0 - 15
        /// @param _index register index
        /// @return register by reference
        u32& get_register(const u32& _index);
        
        /// @brief check condition from opcode
        /// @param _code condition masked opcode
        /// @return true if opcode can be ran
        const bool check_condition(const u32& _code);

        const u32 shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift, u32& _carryFlag);
        const u32 shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift);

    private:
        const u32 arm_dataproc(const u32& _opcode);
        const u32 arm_branch(const u32& _opcode);
        const u32 arm_branch_ex(const u32& _opcode);
        const u32 arm_trans_single(const u32& _opcode);

    private:
        /// @brief setup the armISA instruction map
        void create_arm_isa();

        /// @brief reset all registers to zero
        void reset_registers();

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

        // arm instruction set array
        std::array<cpu_instruction, ARM_ISA_COUNT> armISA;

    private:
        // connection to gba bus for memory reading and writing
        bus& addressBus;

    private:
        std::string debugLog;

    public:
        cpu(bus& _addressBus);
    };
}