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

        std::string debug_info;
    };

    enum struct cpu_mode : u32
    {
        FIQ = 1,
        IRQ,
        SUPERVISOR,
        ABORT,
        UNDEFINED,
        SYSTEM = 0xFE,
        USER = 0xFF
    };

    enum struct cpu_exception : u32
    {
        RESET,
        UNDEFINED,
        SWI,
        PREFETCH_ABORT,
        DATA_ABORT,
        IRQ,
        FIQ
    };

    class cpu
    {
    public:
        /// @brief step the cpu
        /// @return cycle count
        const u32 cycle();

        /// @brief reset the cpu
        void reset();

        /// @brief trigger an external interrupt
        void interrupt();

        /// @brief trigger a fast external interrupt
        void fast_interrupt();

    public:
        /// @brief print status information of the cpu, for debug purposes
        /// @return formatted status information
        const std::string debug_print_status();

        void debug_save_log(const std::string& _filePath);

        void debug_log_arm_cycle(const u32& _opcode, const cpu_instruction& _instruction);

        const std::string debug_print_isa(const bool& _armISA);

        const std::string debug_print_status_registers();

    private:
        /// @brief decode 32-bit arm instruction
        /// @return cycle count
        const u32 decode_arm_instruction();
        /// @brief decode 16-bit thumb instruction
        /// @return cycle count
        const u32 decode_thumb_instruction();   

        /// @brief get registers 0 - 15
        /// @param _index register index
        /// @param _forceUser retreive user registers instead of current banked registers
        /// @return register by reference
        u32& get_register(const u32& _index, const bool& _forceUser = false);

        /// @brief get saved program status register
        /// @param _isUserMode true when cpu is in user/elevated user mode
        /// @return reference to spsr when not in user mode, otherwise the current psr
        u32& get_current_spsr(bool& _isUserMode);

        /// @brief get the current elevation mode of the cpu
        /// @return cpu_mode enum of current mode
        const cpu_mode get_current_mode();

        void set_current_mode(const cpu_mode& _mode);
        
        /// @brief check condition from opcode
        /// @param _code condition masked opcode
        /// @return true if opcode can be ran
        const bool check_condition(const u32& _code);

        const u32 shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift, u32& _carryFlag);
        const u32 shift_operand(const u32& _shiftType, const bool& _zeroShift, const u32& _operand, const u32& _shift);

        void trigger_exception(const cpu_exception& _exception);

    private:
        const u32 arm_dataproc(const u32& _opcode);
        const u32 arm_branch(const u32& _opcode);
        const u32 arm_branch_ex(const u32& _opcode);
        const u32 arm_trans_single(const u32& _opcode);
        const u32 arm_trans_half(const u32& _opcode);
        const u32 arm_trans_swap(const u32& _opcode);
        const u32 arm_trans_block(const u32& _opcode);
        const u32 arm_multiply(const u32& _opcode);
        const u32 arm_psr(const u32& _opcode);
        const u32 arm_soft_interrupt(const u32& _opcode);

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