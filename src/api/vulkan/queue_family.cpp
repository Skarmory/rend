#include "api/vulkan/queue_family.h"

using namespace rend;

QueueFamily::QueueFamily(uint32_t index, const VkQueueFamilyProperties& properties, bool supports_present) : _index(index), _properties(properties), _supports_present_queue(supports_present)
{
}

uint32_t QueueFamily::get_index(void) const
{
    return _index;
}

const VkQueueFamilyProperties& QueueFamily::get_properties(void) const
{
    return _properties;
}

bool QueueFamily::supports_present_queue(void) const
{
    return _supports_present_queue;
}
