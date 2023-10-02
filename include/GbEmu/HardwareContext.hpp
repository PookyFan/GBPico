#pragma once

#include <array>
#include "GbEmu/commonTypes.hpp"
#include "utils.hpp"

namespace GbEmu
{
enum class HardwareRegisters : std::uint16_t {P1, SB, SC /*more to come*/, IE, Invalid};

constexpr auto HardwareRegistersCount = asIndex(HardwareRegisters::Invalid);

class HardwareContext
{
    public:
        void setHwRegister(HardwareRegisters regId, Byte value);
        Byte getHwRegister(HardwareRegisters regId);

    private:
        std::array<Byte, HardwareRegistersCount> hwRegsValues;
        bool interruptsEnabled;
};
}
