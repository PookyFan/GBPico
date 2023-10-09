#pragma once

#include "GbEmu/commonTypes.hpp"
#include "GbEmu/internalUtils.hpp"
#include "testEnabler.hpp"

namespace GbEmu
{
    class GamePack
    {
        public:
            mockable void write(MemoryAddress addr, Byte value)
            {
                //todo
            }

            mockable Byte read(MemoryAddress addr) const
            {
                //todo
                return {};
            }
    };
}
