#pragma once

#include <array>
#include <cstdint>
#include "utils.hpp"

namespace GbEmu
{
using Register = std::uint8_t;
using DoubleRegister = std::uint16_t;

enum class GeneralRegisters {F, A, C, B, E, D, L, H};
enum class SpecialRegisters {PC, SP};
enum class PairRegisters    {AF = asIndex(GeneralRegisters::F),
                             BC = asIndex(GeneralRegisters::C),
                             DE = asIndex(GeneralRegisters::E),
                             HL = asIndex(GeneralRegisters::L)};

constexpr auto GeneralRegistersCount = asIndex(GeneralRegisters::H) + 1;
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

        DoubleRegister& operator[](PairRegisters reg)
        {
            auto index = asIndex(reg);
            return reinterpret_cast<DoubleRegister&>(generalRegisters[index]);
        }

    private:
        std::array<Register, GeneralRegistersCount> generalRegisters;
        std::array<DoubleRegister, SpecialRegistersCount> specialRegisters;
};
}
