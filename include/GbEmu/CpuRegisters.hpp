#pragma once

#include <array>
#include <cstdint>
#include "utils.hpp"

namespace GbEmu
{
using Register = std::uint_fast8_t;
using DoubleRegister = std::uint_fast16_t;

enum class GeneralRegisters {A, B, C, D, E, H, L};
enum class SpecialRegisters {PC, SP};
enum class PairRegisters    {BC = asIndex(GeneralRegisters::B),
                             DE = asIndex(GeneralRegisters::D),
                             HL = asIndex(GeneralRegisters::H)};

constexpr auto GeneralRegistersCount = asIndex(GeneralRegisters::L) + 1;
constexpr auto SpecialRegistersCount = asIndex(SpecialRegisters::SP) + 1;

class CpuRegisters
{
    public:
        Register& operator[](GeneralRegisters reg)
        {
            return generalRegisters[asIndex(reg)];
        }

        DoubleRegister& operator[](SpecialRegisters reg)
        {
            return specialRegisters[asIndex(reg)];
        }

        DoubleRegister operator[](PairRegisters reg) const
        {
            auto index = asIndex(reg);
            return (generalRegisters[index] << 8) | generalRegisters[index + 1];
        }

    private:
        std::array<Register, GeneralRegistersCount> generalRegisters;
        std::array<DoubleRegister, SpecialRegistersCount> specialRegisters;
};
}
