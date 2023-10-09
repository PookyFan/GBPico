#pragma once

#include <array>

#include "GbEmu/GamePack.hpp"
#include "GbEmu/HardwareContext.hpp"
#include "GbEmu/internalUtils.hpp"
#include "utils.hpp"

namespace GbEmu
{
class GenericMemoryRegion
{
    public:
        virtual void write(MemoryAddress addr, Byte value) = 0;
        virtual Byte read(MemoryAddress addr) const = 0;
};

class DummyMemoryRegion : public GenericMemoryRegion
{
    static constexpr Byte dummyValue = 0;
    public:
        void write(MemoryAddress, Byte) override
        {
        }

        Byte read(MemoryAddress) const override
        {
            return dummyValue;
        }
};

class GamePackRegion : public GenericMemoryRegion
{
    public:
        GamePackRegion(GamePack& gp) : gamePack(gp)
        {}

        void write(MemoryAddress addr, Byte value) override
        {
            gamePack.write(addr, value);
        }

        Byte read(MemoryAddress addr) const override
        {
            return gamePack.read(addr);
        }
    private:
        GamePack& gamePack;
};

class HwRegistersRegion : public GenericMemoryRegion
{
    public:
        HwRegistersRegion(HardwareContext& hwCtxt) : hwContext(hwCtxt)
        {}

        void write(MemoryAddress addr, Byte value) override
        {
            auto regId = getHwRegIdByAddress(addr);
            hwContext.setHwRegister(regId, value);
        }

        Byte read(MemoryAddress addr) const override
        {
            auto regId = getHwRegIdByAddress(addr);
            return hwContext.getHwRegister(regId);
        }

    private:
        HardwareContext& hwContext;
};

template<unsigned Size>
class StaticMemoryRegion : public GenericMemoryRegion
{
    public:
        StaticMemoryRegion(MemoryAddress base) : storage(), baseAddress(base)
        {}

        void write(MemoryAddress addr, Byte value) override
        {
            storage[getIndex(addr)] = value;
        }

        Byte read(MemoryAddress addr) const override
        {
            return storage[getIndex(addr)];
        }

    protected:
        using Index = MemoryAddress;

        virtual Index getIndex(MemoryAddress addr) const
        {
            return addr -= baseAddress;
        }

        std::array<Byte, Size> storage;
        MemoryAddress baseAddress;
};

template<unsigned Size, unsigned BanksCount>
class BankedMemoryRegion : public StaticMemoryRegion<Size * BanksCount>
{
    using BaseT = StaticMemoryRegion<Size * BanksCount>;
    public:
        BankedMemoryRegion(MemoryAddress base) : BaseT(base), currentBankNumber(0)
        {
        }

        void changeBank(unsigned newBankNumber)
        {
            currentBankNumber = newBankNumber;
        }

    private:
        unsigned currentBankNumber;

        typename BaseT::Index getIndex(MemoryAddress addr) const override
        {
            return currentBankNumber * Size + BaseT::getIndex(addr);
        }
};
}
