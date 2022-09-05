#ifndef REND_CORE_DRAW_ITEM_H
#define REND_CORE_DRAW_ITEM_H

#include "core/rend_defs.h"

#include <glm/glm.hpp>
#include <vector>

namespace rend
{

struct PerViewData
{
    BufferHandle camera_data_uniform_buffer_h;
    BufferHandle light_data_uniform_buffer_h;
    DescriptorSetHandle descriptor_set;
};

struct PerPassData
{
    FramebufferHandle   framebuffer;
    ColourClear         colour_clear;
    DepthStencilClear   depth_clear;
    RenderArea          render_area;
};

struct PerDrawData // Push constant
{
    void* data;
    size_t bytes;
    ShaderStages stages;
};

struct DrawItem
{
    MeshHandle mesh{ NULL_HANDLE };
    MaterialHandle material{ NULL_HANDLE };
    PerDrawData per_draw_data;
};

}

#endif

