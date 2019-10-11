#ifndef REND_DESCRIPTOR_POOL_H
#define REND_DESCRIPTOR_POOL_H

#include <vulkan.h>
#include <vector>

namespace rend
{

class DeviceContext;
class DescriptorPool;
class DescriptorSetLayout;

class DescriptorSet
{
public:
    DescriptorSet(DeviceContext& device, VkDescriptorSet set);
    ~DescriptorSet(void) = default;

    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    VkDescriptorSet get_handle(void) const;

    // Describe binding for given image(s) to given binding point at given array elem
    void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkDescriptorImageInfo>& descriptor_infos);
    void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const VkDescriptorImageInfo* data, uint32_t count);

    // Describe binding for given buffer(s) to given binding point at given array elem
    void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkDescriptorBufferInfo>& descriptor_infos);
    void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const VkDescriptorBufferInfo* data, uint32_t count);

    // Describe binding for given texel buffer(s) to given binding point at given array elem
    void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkBufferView>& descriptor_infos);

    // Update the descriptor set bindings. Call this when done describing the descriptor set
    void update(void);


private:
    DeviceContext& _context;

    VkDescriptorSet _vk_set;
    std::vector<VkWriteDescriptorSet> _vk_write_descs;
};

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
    VkDescriptorPool _vk_pool;

    std::vector<DescriptorSet*> _sets;
};

}

#endif
