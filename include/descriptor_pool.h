#ifndef DESCRIPTOR_POOL_H
#define DESCRIPTOR_POOL_H

#include <vulkan/vulkan.h>
#include <vector>

namespace rend
{

class LogicalDevice;
class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet
{
    friend class DescriptorPool;

public:
    DescriptorSet(const DescriptorSet&) = delete;
    DescriptorSet(DescriptorSet&&) = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&) = delete;

    VkDescriptorSet get_handle(void) const;

private:
    DescriptorSet(VkDescriptorSet set);
    ~DescriptorSet(void) = default;

private:
    VkDescriptorSet _vk_set;
};

class DescriptorPool
{
    friend class LogicalDevice;

public:

    DescriptorPool(const DescriptorPool&) = delete;
    DescriptorPool(DescriptorPool&&)      = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&)      = delete;

    VkResult allocate(const std::vector<DescriptorSetLayout*>& layouts, std::vector<DescriptorSet*>& out_sets);

private:

    DescriptorPool(LogicalDevice* device, uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes);
    ~DescriptorPool(void);

private:
    VkDescriptorPool _vk_pool;

    LogicalDevice* _device;
    const uint32_t _max_sets;
    uint32_t       _sets_allocated;
};

}

#endif
