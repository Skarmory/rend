#ifndef REND_CORE_DESCRIPTOR_UPDATE_RATE_H
#define REND_CORE_DESCRIPTOR_UPDATE_RATE_H

#include <cstdint>
#include <cstddef>

namespace rend
{
    /** These values correspond with the shader layout "set" bindings.
     */
    enum DescriptorUpdateRate
    {
        Static      = 0,
        PerFrame    = 1,
        PerPass     = 2,
        PerMaterial = 3,
        PerDraw     = 4,
        Count
    };
}

#endif
