#ifndef REND_CORE_LOGGING_LOG_HELPER_FUNCS_H
#define REND_CORE_LOGGING_LOG_HELPER_FUNCS_H

#include <string>

#include "core/rend_defs.h"

namespace rend
{

struct AttachmentInfo;
struct BufferInfo;
struct FramebufferInfo;
struct PerPassData;
struct RenderPassInfo;
struct SubPassDescription;
struct TextureInfo;

}

namespace rend::core::logging
{

std::string to_string(const AttachmentInfo& info);
std::string to_string(const BufferInfo& info);
std::string to_string(BufferUsage usage);
std::string to_string(const DepthStencilClear& clear);
std::string to_string(const rend::FramebufferInfo& info);
std::string to_string(const PerPassData& ppd);
std::string to_string(PipelineBindPoint bind_point);
std::string to_string(const RenderArea& area);
std::string to_string(const RenderPassInfo& info);
std::string to_string(const RGBA& rgba);
std::string to_string(const SubPassDescription& subpass_description);
std::string to_string(const TextureInfo& info);

}

#endif
