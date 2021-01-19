#ifndef QUEUE_FAMILY_H
#define QUEUE_FAMILY_H

#include <vulkan.h>

namespace rend
{

enum class QueueType
{
    GRAPHICS,
    TRANSFER
};

class QueueFamily
{
public:
    QueueFamily(uint32_t index, const VkQueueFamilyProperties& properties, bool supports_present);
    ~QueueFamily(void) = default;

    uint32_t get_index(void) const;
    const VkQueueFamilyProperties& get_properties(void) const;
    bool supports_present_queue(void) const;

private:
    uint32_t _index{ 0 };
    VkQueueFamilyProperties _properties{};
    bool _supports_present_queue{ false };
};

}

#endif
