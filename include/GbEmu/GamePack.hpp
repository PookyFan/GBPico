#pragma once

#include "GbEmu/commonTypes.hpp"
#include "GbEmu/internalUtils.hpp"

namespace GbEmu
{
    class GamePack
    {
        public:
            void write(MemoryAddress addr, Byte value)
            {
                //todo
            }

            MemoryAccessResult read(MemoryAddress addr) const
            {
                //todo
                return {};
            }
    };
}
