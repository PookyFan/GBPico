#pragma once

#include "GbEmu/MemoryClasses.hpp"

namespace GbEmu
{
class MemoryContext
{
    struct MemoryOperationOperands
    {
        GenericMemoryRegion& region;
        MemoryAddress address;

        void writeMemory(Byte value)
        {
            region.write(address, value);
        }

        MemoryAccessResult readMemory()
        {
            return region.read(address);
        }
    };

    public:
        MemoryContext(GamePack& gp, HardwareContext& hwCtxt);
        void writeMemory(MemoryAddress addr, Byte value);
        MemoryAccessResult readMemory(MemoryAddress addr);

    private:
    //todo: fields order may impact final size of the class, for now their order is per memory map from manual
        GamePackRegion             gameMemory;
        BankedMemoryRegion<8kb, 2> lcdMemory;
        StaticMemoryRegion<4kb>    staticWorkingMemory;
        BankedMemoryRegion<4kb, 7> bankedWorkingMemory;
        StaticMemoryRegion<160>    objectsMemory;
        DummyMemoryRegion          notUsableMemory;
        HwRegistersRegion          hwRegistersMemory;
        StaticMemoryRegion<127>    highRamMemory;

        MemoryOperationOperands getMemoryOperationOperands(MemoryAddress addr);
        GenericMemoryRegion&    getMemoryRegion(MemoryAddress addr);
};
}
