#include <algorithm>
#include <array>
#include <cstdint>
#include "GbEmu/internalUtils.hpp"

namespace GbEmu
{
namespace
{
struct HwRegInfo
{
    HardwareRegisters id;
    std::uint16_t address;
};

std::array<HwRegInfo, HardwareRegistersCount> hwRegsInfo {{
    {HardwareRegisters::P1, 0xFF00},
    {HardwareRegisters::SB, 0xFF01},
    {HardwareRegisters::SC, 0xFF02},
    //...
    {HardwareRegisters::IE, 0xFFFF}
}};
}

HardwareRegisters getHwRegIdByAddress(MemoryAddress address)
{
    auto infoIt = std::lower_bound(hwRegsInfo.begin(), hwRegsInfo.end(), address,
                             [](const HwRegInfo& i, MemoryAddress a) {return i.address < a;});
    if(infoIt == hwRegsInfo.end() || infoIt->address != address)
        return HardwareRegisters::Invalid;
    return infoIt->id;
}

}
