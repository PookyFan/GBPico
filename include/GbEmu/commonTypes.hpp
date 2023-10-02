#pragma once

#include <cstdint>

namespace GbEmu
{
using Byte = std::uint8_t;
using MemoryAddress = std::uint_fast16_t;

struct MemoryAccessResult
{
    const bool completed;
    const Byte value;

    MemoryAccessResult() : completed(false), value() {}
    MemoryAccessResult(Byte val) : completed(true), value(val) {}
};
}
