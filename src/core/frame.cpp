#include "core/frame.h"

#include "core/framebuffer.h"
#include "core/gpu_texture.h"

using namespace rend;

GPUTexture* FrameData::find_render_target(const std::string& name) const
{
    for(auto& rt : render_targets)
    {
        if(rt->name() == name)
        {
            return rt;
        }
    }

    return nullptr;
}

Framebuffer* FrameData::find_framebuffer(const std::string& name) const
{
    for(auto& framebuffer : framebuffers)
    {
        if(framebuffer->name() == name)
        {
            return framebuffer;
        }
    }

    return nullptr;
}

