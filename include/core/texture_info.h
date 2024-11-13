#ifndef REND_CORE_TEXTURE_INFO_H
#define REND_CORE_TEXTURE_INFO_H

#include "core/rend_defs.h"

namespace rend
{
    struct TextureInfo
    {
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        uint32_t depth{ 0 };
        bool use_size_ratio{ false };
        SizeRatio size_ratio{}; // Overrides width, height, sets to some ratio of the full screen
        uint32_t mips{ 0 };
        uint32_t layers{ 0 };
        Format format{ Format::R8G8B8A8 };
        ImageLayout layout{ ImageLayout::UNDEFINED };
        MSAASamples samples{ MSAASamples::MSAA_1X };
        ImageUsage usage{ ImageUsage::NONE };
    };
}

#endif
