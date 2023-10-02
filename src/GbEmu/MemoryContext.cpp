
#include "GbEmu/MemoryContext.hpp"

namespace GbEmu
{
namespace
{
constexpr auto echoMemoryMask = 0xDFFF;

constexpr auto fixEchoMemoryAddress(MemoryAddress addr)
{
    return addr &= echoMemoryMask;
}
}

void MemoryContext::writeMemory(MemoryAddress addr, Byte value)
{
   getMemoryOperationOperands(addr).writeMemory(value);
}

MemoryAccessResult MemoryContext::readMemory(MemoryAddress addr)
{
    return getMemoryOperationOperands(addr).readMemory();
}

MemoryContext::MemoryOperationOperands MemoryContext::getMemoryOperationOperands(MemoryAddress addr)
{
    auto& memoryRegion = getMemoryRegion(addr);
    if(&memoryRegion == &staticWorkingMemory || &memoryRegion == &bankedWorkingMemory)
        addr = fixEchoMemoryAddress(addr);
    return {memoryRegion, addr};
}

GenericMemoryRegion& MemoryContext::getMemoryRegion(MemoryAddress addr)
{
    //As per chapter 1.4 from manual

    if(addr < 0x8000)
        return gameMemory;
    
    if(addr < 0xC000)
    {
        if(addr < 0xA000)
            return lcdMemory;
        return gameMemory;
    }

    if(addr < 0xFE00)
    {
        addr = fixEchoMemoryAddress(addr);
        if(addr < 0xD000)
            return staticWorkingMemory;
        return bankedWorkingMemory;
    }

    if(addr < 0xFEA0)
        return objectsMemory;
    
    if(addr < 0xFF00)
        return notUsableMemory;
    
    if(addr >= 0xFF80 && addr <= 0xFFFE)
        return highRamMemory;
    
    return hwRegistersMemory;
}
}
