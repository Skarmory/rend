#ifndef REND_CONSTANTS_H
#define REND_CONSTANTS_H

#include <cstdint>
#include <cstddef>

namespace rend
{

namespace constants
{
    constexpr size_t max_blend_constants{ 4 };
    constexpr size_t max_dynamic_states{ 8 };
    constexpr size_t max_framebuffer_attachments{ 8 };
    constexpr size_t max_subpasses{ 8 };
    constexpr size_t max_vertex_attributes{ 8 };
    constexpr size_t max_viewports{ 8 };
    constexpr size_t max_scissors{ 8 };
}

}

#endif
