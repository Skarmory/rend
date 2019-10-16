#ifndef REND_DESCRIPTOR_POOL_H
#define REND_DESCRIPTOR_POOL_H

#include <vector>
#include <vulkan.h>

namespace rend
{

class DeviceContext;
class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
public:
    DescriptorPool(DeviceContext& context);
    ~DescriptorPool(void);
    DescriptorPool(const DescriptorPool&)            = delete;
    DescriptorPool(DescriptorPool&&)                 = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&)      = delete;

    bool create_descriptor_pool(uint32_t max_sets, const std::vector<VkDescriptorPoolSize>& pool_sizes);

    VkResult allocate(const std::vector<DescriptorSetLayout*>& layouts, std::vector<DescriptorSet*>& out_sets);

private:
    DeviceContext& _context;
    uint32_t _max_sets;
    std::vector<DescriptorSet*> _sets;

    VkDescriptorPool _vk_pool;
};

}

#endif
