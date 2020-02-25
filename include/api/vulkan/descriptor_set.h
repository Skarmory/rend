#ifndef REND_DESCRIPTOR_SET_H
#define REND_DESCRIPTOR_SET_H

#include <variant>
#include <vector>
#include <vulkan.h>

namespace rend
{

class VulkanGPUBuffer;
class VulkanGPUTexture;
class VulkanUniformBuffer;

class DescriptorSet
{
    struct Binding;

public:
    explicit DescriptorSet(VkDescriptorSet set);
    ~DescriptorSet(void) = default;

    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    VkDescriptorSet get_handle(void) const;

    // Describe binding for given image(s) to given binding point at given array elem
    void describe(uint32_t binding, VulkanGPUTexture* texture);

    // Describe binding for given buffer(s) to given binding point at given array elem
    void describe(uint32_t binding, VulkanUniformBuffer* buffer);

    // Describe binding for given texel buffer(s) to given binding point at given array elem
    //void describe(uint32_t binding, uint32_t array_elem, VkDescriptorType type, const std::vector<VkBufferView>& descriptor_infos);

    // Update the descriptor set bindings. Call this when done describing the descriptor set
    void update(void);

private:
    Binding* _find_binding(uint32_t slot);

private:
    struct Binding
    {
        uint32_t slot;
        VkDescriptorType type;
        std::variant<VulkanGPUTexture*, VulkanGPUBuffer*> bound_resource;
    };

    std::vector<Binding> _bindings;

    VkDescriptorSet _vk_set;
};

}

#endif
