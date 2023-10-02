#pragma once

#include <bitset>
#include "GbEmu/CpuRegisters.hpp"
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

constexpr auto CpuFlagsCount = asIndex(CpuFlags::Carry) + 1;

class CpuContext
{
    public:
        void tick();

    private:
        CpuRegisters cpuRegisters;
        std::bitset<CpuFlagsCount> cpuFlags;
        unsigned int currentInstructionCycle;
};
}