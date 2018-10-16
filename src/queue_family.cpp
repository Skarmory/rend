#include "queue_family.h"

#include <iostream>

using namespace rend;

QueueFamily::QueueFamily(uint32_t index, const VkQueueFamilyProperties& properties, bool supports_present) : _index(index), _properties(properties), _supports_present_queue(supports_present)
{
    std::cout << "Constructing queue family" << std::endl;
}

QueueFamily::~QueueFamily(void)
{
    std::cout << "Destructing queue family" << std::endl;
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
