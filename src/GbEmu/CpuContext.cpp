
#include "GbEmu/CpuContext.hpp"
#include "GbEmu/MemoryContext.hpp"
#include "GbEmu/HardwareContext.hpp"

namespace GbEmu
{
namespace
{
enum InstructionGroup {Group00 = 0b00000000, Group01 = 0b01000000, Group10 = 0b10000000, Group11 = 0b11000000};

enum ImmediatesCount {NoImmediates, OneImmediate, TwoImmediates};

enum CyclesCount {OneCycle = 1, TwoCycles, ThreeCycles, FourCycles, FiveCycles, SixCycles};

constexpr Byte instructionGroupMask = 0b11000000;
constexpr Byte bitTripletMask = 0b00000111;
constexpr Byte tripletOperandHL = 0b110;

auto getFlagBit(CpuFlags flag)
{
    return 1 << asIndex(flag);
}

InstructionContext decodeGroup00(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet);
InstructionContext decodeGroup01(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet);
InstructionContext decodeGroup10(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet);
InstructionContext decodeGroup11(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet);
}

void CpuContext::tick()
{
    ++totalTicksCount;
    switch(currentState)
    {
        case CpuState::FetchAndDecode:
        {
            auto addr = cpuRegisters[SpecialRegisters::PC];
            decode(memCtxt.readMemory(addr)); //todo: fetch is technically parallel to executing previous instruction
            //todo: invalid instruction -> lock CPU (maybe during execution phase...?)
            break;
        }
        // ...
    }
    //todo
}

inline bool CpuContext::isAtFullCycle()
{
    return (totalTicksCount % clockDivider) == 0;
}

bool CpuContext::isFlagSet(CpuFlags flagToCheck)
{
    return (cpuRegisters[GeneralRegisters::F] & getFlagBit(flagToCheck)) != 0;
}

void CpuContext::decode(const Byte instruction)
{
    const InstructionGroup group = static_cast<InstructionGroup>(instruction & instructionGroupMask);
    const unsigned lowTriplet = instruction & bitTripletMask;
    const unsigned highTriplet = (instruction >> 3) & bitTripletMask;
    switch(group)
    {
        case Group00:
        {
            instructionCtxt = decodeGroup00(instruction, group, lowTriplet, highTriplet);
            break;
        }
        case Group01:
        {
            instructionCtxt = decodeGroup01(instruction, group, lowTriplet, highTriplet);
            break;
        }
        case Group10:
        {
            instructionCtxt = decodeGroup10(instruction, group, lowTriplet, highTriplet);
            break;
        }
        case Group11:
        {
            instructionCtxt = decodeGroup11(instruction, group, lowTriplet, highTriplet);
            break;
        }
    }
}

namespace
{
inline auto getOperandFromTriplet_aaX(const unsigned triplet)
{
    return (triplet >> 1) & 0b11;
}

inline auto getOperandFromTriplet_Xaa(const unsigned triplet)
{
    return triplet & 0b11;
}

InstructionContext decodeGroup00(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet)
{
    switch(lowTriplet)
    {
        case 0b000:
        {
            switch(highTriplet)
            {
                case 0b000: return {instruction, Mnemonic::NOP};
                case 0b001: return {instruction, Mnemonic::LD_addrnn_SP, TwoImmediates, FiveCycles};
                case 0b010: return {instruction, Mnemonic::STOP};
                case 0b011: return {instruction, Mnemonic::JR_e,    OneImmediate, ThreeCycles};
                default:    return {instruction, Mnemonic::JR_cc_e, OneImmediate, TwoCycles, getOperandFromTriplet_Xaa(highTriplet)};
            }
        }
        case 0b001:
        {
            auto operand = getOperandFromTriplet_aaX(highTriplet);
            if((highTriplet & 1) == 0)
                return {instruction, Mnemonic::LD_dd_nn, TwoImmediates, ThreeCycles, operand};
            return {instruction, Mnemonic::ADD_HL_ss, NoImmediates, TwoCycles, operand};
        }
        case 0b010:
        {
            switch(highTriplet)
            {
                case 0b000: return {instruction, Mnemonic::LD_addrBC_A,  NoImmediates, TwoCycles};
                case 0b001: return {instruction, Mnemonic::LD_A_addrBC,  NoImmediates, TwoCycles};
                case 0b010: return {instruction, Mnemonic::LD_addrDE_A,  NoImmediates, TwoCycles};
                case 0b011: return {instruction, Mnemonic::LD_A_addrDE,  NoImmediates, TwoCycles};
                case 0b100: return {instruction, Mnemonic::LD_addrHLI_A, NoImmediates, TwoCycles};
                case 0b101: return {instruction, Mnemonic::LD_A_addrHLI, NoImmediates, TwoCycles};
                case 0b110: return {instruction, Mnemonic::LD_addrHLD_A, NoImmediates, TwoCycles};
                case 0b111: return {instruction, Mnemonic::LD_A_addrHLD, NoImmediates, TwoCycles};
            }
        }
        case 0b011:
        {
            auto operand = getOperandFromTriplet_aaX(highTriplet);
            auto mnemonic = ((highTriplet & 1) == 0) ? Mnemonic::INC_ss : Mnemonic::DEC_ss;
            return {instruction, mnemonic, NoImmediates, TwoCycles, operand};
        }
        case 0b100:
        {
            if(highTriplet == tripletOperandHL)
                return {instruction, Mnemonic::INC_addrHL, NoImmediates, ThreeCycles};
            return {instruction, Mnemonic::INC_r, NoImmediates, OneCycle, highTriplet};
        }
        case 0b101:
        {
            if(highTriplet == tripletOperandHL)
                return {instruction, Mnemonic::DEC_addrHL, NoImmediates, ThreeCycles};
            return {instruction, Mnemonic::DEC_r, NoImmediates, OneCycle, highTriplet};
        }
        case 0b110:
        {
            if(highTriplet == tripletOperandHL)
                return {instruction, Mnemonic::LD_addrHL_n, OneImmediate, ThreeCycles};
            return {instruction, Mnemonic::LD_r_n, OneImmediate, TwoCycles, highTriplet};
        }
        case 0b111:
        {
            switch(highTriplet)
            {
                case 0b000: return {instruction, Mnemonic::RLCA};
                case 0b001: return {instruction, Mnemonic::RRCA};
                case 0b010: return {instruction, Mnemonic::RLA,};
                case 0b011: return {instruction, Mnemonic::RRA};
                case 0b100: return {instruction, Mnemonic::DAA};
                case 0b101: return {instruction, Mnemonic::CPL};
                case 0b110: return {instruction, Mnemonic::SCF};
                case 0b111: return {instruction, Mnemonic::CCF};
            }
        }
    }
    return {instruction, Mnemonic::Invalid}; //We shouldn't ever get here
}

InstructionContext decodeGroup01(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet)
{
    if(lowTriplet == highTriplet && lowTriplet == 0b110)
        return {instruction, Mnemonic::HALT};

    if(lowTriplet == tripletOperandHL)
        return {instruction, Mnemonic::LD_r_addrHL, NoImmediates, TwoCycles};

    if(highTriplet == tripletOperandHL)
        return {instruction, Mnemonic::LD_addrHL_r, NoImmediates, TwoCycles};

    return {instruction, Mnemonic::LD_r1_r2};
}

InstructionContext decodeGroup10(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet)
{
    switch(highTriplet)
    {
        case 0b000:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::ADD_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::ADD_A_r};
        }
        case 0b001:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::ADC_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::ADC_A_r};
        }
        case 0b010:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::SUB_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::SUB_A_r};
        }
        case 0b011:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::SBC_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::SBC_A_r};
        }
        case 0b100:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::AND_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::AND_A_r};
        }
        case 0b101:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::XOR_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::XOR_A_r};
        }
        case 0b110:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::OR_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::OR_A_r};
        }
        case 0b111:
        {
            if(lowTriplet == tripletOperandHL)
                return {instruction, Mnemonic::CP_A_addrHL, NoImmediates, TwoCycles};
            return {instruction, Mnemonic::CP_A_r};
        }
    }
    return {instruction, Mnemonic::Invalid}; //We shouldn't ever get here
}

InstructionContext decodeGroup11(const Byte instruction, const InstructionGroup group, const unsigned lowTriplet, const unsigned highTriplet)
{
    switch(lowTriplet)
    {
        case 0b000:
        {
            switch(highTriplet)
            {
                case 0b100: return {instruction, Mnemonic::LD_addrFFn_A, OneImmediate, ThreeCycles};
                case 0b101: return {instruction, Mnemonic::ADD_SP_e, OneImmediate, FourCycles};
                case 0b110: return {instruction, Mnemonic::LD_A_addrFFn, OneImmediate, ThreeCycles};
                case 0b111: return {instruction, Mnemonic::LDHL_SP_e, OneImmediate, ThreeCycles};
                default:    return {instruction, Mnemonic::RET_cc, NoImmediates, TwoCycles, getOperandFromTriplet_Xaa(highTriplet)};
            }
        }
        case 0b001:
        {
            switch(highTriplet)
            {
                case 0b001: return {instruction, Mnemonic::RET,  NoImmediates, FourCycles};
                case 0b011: return {instruction, Mnemonic::RETI, NoImmediates, FourCycles};
                case 0b101: return {instruction, Mnemonic::JP_HL};
                case 0b111: return {instruction, Mnemonic::LD_SP_HL, NoImmediates, TwoCycles};
                default:    return {instruction, Mnemonic::POP_qq, NoImmediates, ThreeCycles, getOperandFromTriplet_aaX(highTriplet)};
            }
        }
        case 0b010:
        {
            switch(highTriplet)
            {
                case 0b100: return {instruction, Mnemonic::LD_addrC_A, NoImmediates, TwoCycles};
                case 0b101: return {instruction, Mnemonic::LD_addrnn_A, TwoImmediates, FourCycles};
                case 0b110: return {instruction, Mnemonic::L_A_addrC, NoImmediates, TwoCycles};
                case 0b111: return {instruction, Mnemonic::LD_A_addrnn, TwoImmediates, FourCycles};
                default:    return {instruction, Mnemonic::JP_cc_nn, TwoImmediates, ThreeCycles};
            }
        }
        case 0b011:
        {
            switch(highTriplet)
            {
                case 0b000: return {instruction, Mnemonic::JP_nn, TwoImmediates, FourCycles};
                case 0b001: return {instruction, Mnemonic::Extended, OneImmediate, TwoCycles};
                case 0b110: return {instruction, Mnemonic::DI};
                case 0b111: return {instruction, Mnemonic::EI};
                default:    return {instruction, Mnemonic::Invalid};
            }
        }
        case 0b100:
        {
            if(highTriplet < 0b100)
                return {instruction, Mnemonic::CALL_cc_nn, TwoImmediates, ThreeCycles, highTriplet};
            return {instruction, Mnemonic::Invalid};
        }
        case 0b101:
        {
            if(highTriplet & 0x1 == 0)
                return {instruction, Mnemonic::PUSH_qq, NoImmediates, FourCycles};
            else if(highTriplet == 0b001)
                return {instruction, Mnemonic::CALL_nn, TwoImmediates, SixCycles};
            return {instruction, Mnemonic::Invalid};
        }
        case 0b110:
        {
            switch(highTriplet)
            {
                case 0b000: return {instruction, Mnemonic::ADD_A_n, OneImmediate, TwoCycles};
                case 0b001: return {instruction, Mnemonic::ADC_A_n, OneImmediate, TwoCycles};
                case 0b010: return {instruction, Mnemonic::SUB_A_n, OneImmediate, TwoCycles};
                case 0b011: return {instruction, Mnemonic::SBC_A_n, OneImmediate, TwoCycles};
                case 0b100: return {instruction, Mnemonic::AND_A_n, OneImmediate, TwoCycles};
                case 0b101: return {instruction, Mnemonic::XOR_A_n, OneImmediate, TwoCycles};
                case 0b110: return {instruction, Mnemonic::OR_A_n,  OneImmediate, TwoCycles};
                case 0b111: return {instruction, Mnemonic::CP_A_n,  OneImmediate, TwoCycles};
            }
        }
        case 0b111: return {instruction, Mnemonic::RST_t, NoImmediates, FourCycles};
    }
    return {instruction, Mnemonic::Invalid}; //We shouldn't ever get here
}
}
}