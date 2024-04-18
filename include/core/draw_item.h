#ifndef REND_CORE_DRAW_ITEM_H
#define REND_CORE_DRAW_ITEM_H

#include "core/rend_defs.h"

#include <glm/glm.hpp>
#include <vector>

namespace rend
{

class DescriptorSet;
class Framebuffer;
class GPUBuffer;
class Material;
class Mesh;

struct PerViewData
{
    GPUBuffer*     camera_data_uniform_buffer;
    GPUBuffer*     light_data_uniform_buffer;
    DescriptorSet* descriptor_set;
};

struct PerPassData
{
    Framebuffer*      framebuffer;
    ColourClear       colour_clear;
    DepthStencilClear depth_clear;
    RenderArea        render_area;
};

struct PerDrawData // Push constant
{
    void* data;
    size_t bytes;
    ShaderStages stages;
};

struct DrawItem
{
    Mesh* mesh{ nullptr };
    Material* material{ nullptr };
    PerDrawData per_draw_data;
};

}

#endif
