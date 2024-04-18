#include "api/vulkan/event.h"

#include "api/vulkan/vulkan_helper_funcs.h"
#include "api/vulkan/vulkan_device_context.h"

using namespace rend;

Event::Event(VulkanDeviceContext& ctx)
    :
        _ctx(&ctx)
{
    VkEventCreateInfo create_info = vulkan_helpers::gen_event_create_info();
    _vk_event = _ctx->create_event(create_info);
}

Event::~Event(void)
{
    _ctx->destroy_event(_vk_event);
}

VkEvent Event::vk_handle(void) const
{
    return _vk_event;
}
