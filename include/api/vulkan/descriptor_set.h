#ifndef REND_DESCRIPTOR_SET_H
#define REND_DESCRIPTOR_SET_H

#include <variant>
#include <vector>
#include <vulkan.h>

namespace rend
{

class GPUBuffer;
class GPUTexture;

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

    // Describe binding for given image(s) for given binding point at given array elem
    void describe_combined_image_sampler(uint32_t binding, GPUTexture* texture);
    void describe_uniform_buffer(uint32_t binding, GPUBuffer* buffer);

    // Update the descriptor set bindings. Call this when done describing the descriptor set
    void update(void);

private:
    Binding* _find_binding(uint32_t slot);

private:
    struct Binding
    {
        uint32_t slot;
        VkDescriptorType type;
        std::variant<GPUTexture*, GPUBuffer*> bound_resource;
    };

    std::vector<Binding> _bindings;

    VkDescriptorSet _vk_set{ VK_NULL_HANDLE };
};

}

#endif
