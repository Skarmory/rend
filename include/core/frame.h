#ifndef REND_CORE_FRAME_H
#define REND_CORE_FRAME_H

#include <vector>

#include "core/draw_item.h"
#include "core/rend_defs.h"

namespace rend
{

class CommandBuffer;
class Semaphore;
class Fence;
class Framebuffer;
class GPUBuffer;

struct FrameData
{
    uint32_t       swapchain_idx{ 0xdeadbeef };
    uint32_t       frame{ 0 };
    CommandBuffer* command_buffer{ nullptr };
    CommandBuffer* other_buffer{ nullptr };
    Semaphore*     acquire_sem{ nullptr };
    Semaphore*     present_sem{ nullptr };
    Semaphore*     other_sem{ nullptr };
    Fence*         submit_fen{ nullptr };
    Framebuffer*   framebuffer;

    std::vector<GPUBuffer*> staging_buffers_used;
    PerViewData             per_view_data;
};

}

#endif
