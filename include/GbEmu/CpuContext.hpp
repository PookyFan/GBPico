#pragma once

#include <bitset>
#include "GbEmu/CpuRegisters.hpp"
#include "GbEmu/commonTypes.hpp"
#include "utils.hpp"

namespace GbEmu
{
class MemoryContext;
class HardwareContext;

enum class CpuFlags {
    Zero,      //Z-flag
    Sub,       //N-flag
    HalfCarry, //H-flag
    Carry      //CY-flag
};

enum class Mnemonic : uint8_t {
    Invalid, Extended,
    NOP, DI, EI, STOP, HALT,
    POP_qq, PUSH_qq,
    LD_addrnn_SP, LD_dd_nn, LD_addrFFn_A, LD_A_addrFFn, LDHL_SP_e, LD_SP_HL,
    LD_addrHL_n, LD_r_n, LD_r_addrHL, LD_addrHL_r, LD_r1_r2,
    LD_addrC_A, LD_addrnn_A, L_A_addrC, LD_A_addrnn,
    LD_addrBC_A, LD_addrDE_A, LD_addrHLI_A, LD_addrHLD_A, //todo: use same handler for these, only differentiate by instructionCtxt.mnemonic
    LD_A_addrBC, LD_A_addrDE, LD_A_addrHLI, LD_A_addrHLD, //todo: similar as above
    ADD_HL_ss, ADD_A_addrHL, ADD_A_r, ADD_SP_e, ADD_A_n,
    ADC_A_addrHL, ADC_A_r, ADC_A_n,
    SUB_A_addrHL, SUB_A_r, SUB_A_n,
    SBC_A_addrHL, SBC_A_r, SBC_A_n,
    INC_ss, INC_addrHL, INC_r,
    DEC_ss, DEC_addrHL, DEC_r,
    AND_A_addrHL, AND_A_r, AND_A_n,
    XOR_A_addrHL, XOR_A_r, XOR_A_n,
    OR_A_addrHL, OR_A_r, OR_A_n,
    CP_A_addrHL, CP_A_r, CP_A_n,
    DAA, CPL, SCF, CCF,
    RLCA, RRCA, RLA, RRA,
    JP_HL, JP_cc_nn, JP_nn,
    JR_e, JR_cc_e,
    CALL_cc_nn, CALL_nn, RST_t,
    RET, RETI, RET_cc
};

enum class ExtendedMnemonic : uint8_t {
    RLC_m, RRC_m, RL_m, RR_m, SLA_m, SRA_m, SWAP_m, SRL_m, BIT_b_m, RES_b_m, SET_b_m
};

enum class CpuState {FetchAndDecode, Prepare, Execute, Idle, Stopped, Halted, Locked}; //todo: not all may be needed

enum ClockDivider {DividerCGB = 2, DividerDMG = 4};

struct InstructionContext
{
    InstructionContext(const Byte instr, const Mnemonic mnem, const unsigned immediates = 0, const unsigned cycles = 1, const unsigned priv = 0)
        : immediateValue(0), instructionValue(instr), mnemonic(mnem), privateData(priv),
          cyclesCountTarget(cycles), instructionPointerIncrement(immediates + 1), immediatesToFetchCount(immediates)
    {}

    uint16_t immediateValue;
    Byte instructionValue;
    Mnemonic mnemonic;
    uint8_t  privateData;
    uint8_t  cyclesCountTarget;
    uint8_t  instructionPointerIncrement;
    uint8_t  immediatesToFetchCount;
};

class CpuContext
{
    public:
        void tick();

    private:
        CpuRegisters cpuRegisters;
        InstructionContext instructionCtxt;
        MemoryContext& memCtxt;
        HardwareContext& hwCtxt;
        unsigned int currentInstructionCycle;
        unsigned long long totalTicksCount;
        CpuState currentState;
        ClockDivider clockDivider;

        bool isAtFullCycle();
        bool isFlagSet(CpuFlags flagToCheck); //todo: public?

        void decode(const Byte instruction);
};
}