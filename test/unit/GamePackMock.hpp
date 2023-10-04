#pragma once

#include <gmock/gmock.h>
#include "GbEmu/GamePack.hpp"

namespace GbEmu::Test
{
class GamePackMock : public GamePack
{
    public:
        MOCK_METHOD(void, write, (MemoryAddress, Byte), (override));
        MOCK_METHOD(MemoryAccessResult, read, (MemoryAddress), (override, const));
};
}
