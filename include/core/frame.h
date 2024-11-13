#ifndef REND_CORE_FRAME_H
#define REND_CORE_FRAME_H

#include <string>
#include <vector>

#include "core/draw_item.h"
#include "core/rend_defs.h"

namespace rend
{

class CommandBuffer;
class Semaphore;
class SwapchainAcquire;
class Fence;
class Framebuffer;
class GPUBuffer;
class GPUTexture;

struct FrameData
{
    uint32_t       frame{ 0 };
    GPUTexture*    backbuffer_texture{ nullptr };
    CommandBuffer* draw_cmd{ nullptr };
    CommandBuffer* load_cmd{ nullptr };
    //Semaphore*     present_sem{ nullptr };
    Semaphore*     load_sem{ nullptr };
    Fence*         submit_fen{ nullptr };
    SwapchainAcquire* swapchain_acquisition;

    std::vector<Framebuffer*> framebuffers;
    std::vector<GPUTexture*>  render_targets;
    std::vector<GPUBuffer*>   staging_buffers_used;

    [[nodiscard]] GPUTexture*  find_render_target(const std::string& name) const;
    [[nodiscard]] Framebuffer* find_framebuffer(const std::string& name) const;
};

}

#endif
