#pragma once

#include <cstddef>

template<typename T>
constexpr auto asIndex(T val)
{
    return static_cast<std::size_t>(val);
}

constexpr auto operator""kb(unsigned long long sizeInKb)
{
    return sizeInKb * 1024;
}
