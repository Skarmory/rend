#ifndef REND_CORE_DESCRIPTOR_FREQUENCY_H
#define REND_CORE_DESCRIPTOR_FREQUENCY_H

#include <cstdint>
#include <cstddef>

namespace rend
{
    enum DescriptorFrequency
    {
        VIEW = 0,
        MATERIAL = 1
    };
    constexpr size_t DESCRIPTOR_FREQUENCY_COUNT = 2;
}

#endif
