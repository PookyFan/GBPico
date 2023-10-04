#include <gtest/gtest.h>
#include "GbEmu/MemoryContext.hpp"

#include "GamePackMock.hpp"
#include "HardwareContextMock.hpp"

using namespace testing;

namespace GbEmu::Test
{
namespace
{
constexpr Byte VALUE1    = 0x01;
constexpr Byte VALUE2    = 0xFD;
constexpr MemoryAddress GAME_PACK_ADDR_1 = 0x001F;
constexpr MemoryAddress GAME_PACK_ADDR_2 = 0x0100;
constexpr MemoryAddress LCD_MEM_ADDR_1   = 0x8000;
constexpr MemoryAddress LCD_MEM_ADDR_2   = 0x9000;
constexpr MemoryAddress HW_REG_ADDR_1    = 0xFF00;
constexpr MemoryAddress HW_REG_ADDR_2    = 0xFFFF;
}

struct MemoryContextTestFixture : Test
{
    MemoryContextTestFixture() : testedObj(gamePackMock, hwCtxtMock)
    {}

    GamePackMock gamePackMock;
    HardwareContextMock hwCtxtMock;
    MemoryContext testedObj;
};

TEST_F(MemoryContextTestFixture, MemoryCtxtShouldMapMemoryAccessesToGamePackAndHwCtxt)
{
    EXPECT_CALL(gamePackMock, read(GAME_PACK_ADDR_1)).WillRepeatedly(Return(MemoryAccessResult{}));
    EXPECT_CALL(gamePackMock, write(GAME_PACK_ADDR_2, VALUE1));
    EXPECT_CALL(hwCtxtMock, getHwRegister(HardwareRegisters::P1)).WillRepeatedly(Return(VALUE1));
    EXPECT_CALL(hwCtxtMock, setHwRegister(HardwareRegisters::IE, VALUE1));

    testedObj.readMemory(GAME_PACK_ADDR_1);
    testedObj.writeMemory(GAME_PACK_ADDR_2, VALUE1);
    testedObj.readMemory(LCD_MEM_ADDR_1);
    testedObj.writeMemory(LCD_MEM_ADDR_2, VALUE1);
    testedObj.readMemory(HW_REG_ADDR_1);
    testedObj.writeMemory(HW_REG_ADDR_2, VALUE1);
}

TEST_F(MemoryContextTestFixture, MemoryCtxtShouldMapHighMemoryAccessEitherToHwRegsOrHighRam)
{
    EXPECT_CALL(hwCtxtMock, setHwRegister(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(hwCtxtMock, getHwRegister(_)).WillRepeatedly(Return(VALUE1));

    for(MemoryAddress addr = 0xFF00; addr <= 0xFFFF; ++addr)
        testedObj.writeMemory(addr, addr & 0xFF);
    
    for(MemoryAddress addr = 0xFF00; addr < 0xFF80; ++addr)
    {
        auto access = testedObj.readMemory(addr);
        EXPECT_TRUE(access.completed);
        EXPECT_EQ(access.value, VALUE1);
    }

    for(MemoryAddress addr = 0xFF80; addr < 0xFFFF; ++addr)
    {
        auto access = testedObj.readMemory(addr);
        EXPECT_TRUE(access.completed);
        EXPECT_EQ(access.value, addr & 0xFF);
    }

    auto access = testedObj.readMemory(0xFFFF);
    EXPECT_TRUE(access.completed);
    EXPECT_EQ(access.value, VALUE1);
}

}
