#pragma once

#include <gmock/gmock.h>
#include "GbEmu/HardwareContext.hpp"

namespace GbEmu::Test
{
class HardwareContextMock : public HardwareContext
{
    public:
        MOCK_METHOD(void, setHwRegister, (HardwareRegisters, Byte), (override));
        MOCK_METHOD(Byte, getHwRegister, (HardwareRegisters), (override));
};
}
