#include "core/logging/log_helper_funcs.h"

#include "core/draw_item.h"
#include "core/framebuffer.h"
#include "core/gpu_buffer.h"
#include "core/gpu_texture.h"
#include "core/render_pass.h"
#include "core/texture_info.h"

using namespace rend::core::logging;

namespace
{
    std::string buffer_usage_to_string(rend::BufferUsage usage)
    {
        switch(usage)
        {
            case rend::BufferUsage::NONE: return "NONE";
            case rend::BufferUsage::TRANSFER_SRC: return "TRANSFER_SRC";
            case rend::BufferUsage::TRANSFER_DST: return "TRANSFER_DST";
            case rend::BufferUsage::VERTEX_BUFFER: return "VERTEX_BUFFER";
            case rend::BufferUsage::INDEX_BUFFER: return "INDEX_BUFFER";
            case rend::BufferUsage::UNIFORM_BUFFER: return "UNIFORM_BUFFER";
            default: return "";
        }
    }
}

std::string rend::core::logging::to_string(const rend::AttachmentInfo& info)
{
    std::string s = "{ ";
    s += "format: " + FormatNames[(int)info.format] + ", ";
    s += "samples: " + MSAASamplesNames[(int)info.samples] + ", ";
    s += "load_op: " + LoadOpNames[(int)info.load_op] + ", ";
    s += "store_op: " + StoreOpNames[(int)info.store_op] + ", ";
    s += "stencil_load op: " + LoadOpNames[(int)info.stencil_load_op] + ", ";
    s += "stencil_store op: " + StoreOpNames[(int)info.stencil_store_op] + ", ";
    s += "initial_layout: " + ImageLayoutNames[(int)info.initial_layout] + ", ";
    s += "final_layout: " + ImageLayoutNames[(int)info.final_layout];
    s += " }";

    return s;
}

std::string rend::core::logging::to_string(const rend::BufferInfo& info)
{
    std::string s = "{ ";
    s += "element count: " + std::to_string(info.element_count) + ", ";
    s += "element size: " + std::to_string(info.element_size) + ", ";
    s += "usage: " + to_string(info.usage) + " }";
    return s;
}

std::string rend::core::logging::to_string(const rend::DepthStencilClear& clear)
{
    std::string s = "{ depth: " + std::to_string(clear.depth) + ", stencil: " + std::to_string(clear.stencil)  + " }";
    return s;
}

std::string rend::core::logging::to_string(rend::BufferUsage usage)
{
    if(usage == rend::BufferUsage::NONE)
    {
        return "NONE";
    }

    std::string ret = "";
    int flag_check = 1;
    while(flag_check != 0)
    {
        if((flag_check & (int)usage) != 0)
        {
            ret += " " + ::buffer_usage_to_string((rend::BufferUsage)flag_check);
        }

        flag_check <<= 1;
    }

    return ret;
}

std::string rend::core::logging::to_string(const rend::FramebufferInfo& info)
{
    std::string s = "{ ";
    s += "width: " + std::to_string(info.width) + ", ";
    s += "height: " + std::to_string(info.height) + ", ";
    s += "depth: " + std::to_string(info.depth) + ", ";
    s += "layers: " + std::to_string(info.layers) + ", ";
    s += "use size ratio: " + std::to_string(info.use_size_ratio) + ", ";
    s += "size ratio: " + SizeRatioNames[(int)info.use_size_ratio] + ", ";
    s += "render pass: " + info.render_pass->name() + ", ";
    s += "render targets: { ";
    for(auto& rt : info.named_render_targets)
    {
        s += rt + ", ";
    }
    s += " }";
    s += " }";

    return s;
}

std::string rend::core::logging::to_string(const rend::PerPassData& ppd)
{
    std::string s = "{ ";
    s += "framebuffer: " + ppd.framebuffer->name() + ", ";
    s += "attachments : { ";
    for(int i = 0; i < ppd.attachments_count; ++i)
    {
        s += ppd.attachments[i]->name() + ", ";
    }
    s += "}, ";
    s += "colour clear: " + to_string(ppd.colour_clear) + ", ";
    s += "depth clear: " + to_string(ppd.depth_clear) + ", ";
    s += "render area: " + to_string(ppd.render_area);
    s += " }";

    return s;
}

std::string rend::core::logging::to_string(PipelineBindPoint bind_point)
{
    switch(bind_point)
    {
        case PipelineBindPoint::GRAPHICS: return "GRAPHICS";
        case PipelineBindPoint::COMPUTE: return "COMPUTE"; 
        default: return "";
    }
}

std::string rend::core::logging::to_string(const rend::RenderArea& area)
{
    std::string s = "{ w: " + std::to_string(area.w) + ", h: " + std::to_string(area.h) + " }";
    return s;
}

std::string rend::core::logging::to_string(const rend::SubPassDescription& subpass_description)
{
    std::string s = "{ ";

    s += "bind point: " + to_string(subpass_description.bind_point) + ", ";
    s += "colour attachments: { ";
    for(auto& idx : subpass_description.colour_attachment_infos)
    {
        s += std::to_string(idx) + ", ";
    }
    s += "}, ";

    s += "input attachments: { ";
    for(auto& idx : subpass_description.input_attachment_infos)
    {
        s += std::to_string(idx) + ", ";
    }
    s += "}, ";

    s += "resolve attachments: { ";
    for(auto& idx : subpass_description.resolve_attachment_infos)
    {
        s += std::to_string(idx) + ", ";
    }
    s += "}, ";

    s += "preserve attachments: { ";
    for(auto& idx : subpass_description.preserve_attachments)
    {
        s += std::to_string(idx) + ", ";
    }
    s += "}, ";

    s += "depth attachment: " + std::to_string(subpass_description.depth_stencil_attachment);

    s += " }";
    return s;
}

std::string rend::core::logging::to_string(const rend::RenderPassInfo& info)
{
    std::string s = "{ ";

    s += "attachments: { ";
    for(auto& attachment_info : info.attachment_infos)
    {
        s += to_string(attachment_info) + ", ";
    }
    s += " }, ";

    s += "subpasses: { ";
    for(auto& subpass : info.subpasses)
    {
        s+= to_string(subpass) + ", ";
    }
    s+= " }";

    s += " }";

    return s;
}

std::string rend::core::logging::to_string(const rend::RGBA& rgba)
{
    std::string s = "{ r: " + std::to_string(rgba.r) + ", g: " + std::to_string(rgba.g) + ", b:" + std::to_string(rgba.b) + ", a: " + std::to_string(rgba.a) + " }";
    return s;
}

std::string rend::core::logging::to_string(const rend::TextureInfo& info)
{
}
