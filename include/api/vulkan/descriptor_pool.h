#ifndef REND_DESCRIPTOR_POOL_H
#define REND_DESCRIPTOR_POOL_H

#include <vector>
#include <vulkan.h>

#include "rend_defs.h"

namespace rend
{

class DescriptorSet;
class DescriptorSetLayout;

class DescriptorPool
{
public:
    DescriptorPool(void)                             = default;
    ~DescriptorPool(void)                            = default;
    DescriptorPool(const DescriptorPool&)            = delete;
    DescriptorPool(DescriptorPool&&)                 = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&)      = delete;

    StatusCode create(uint32_t max_sets);
    void       destroy(void);

    void set_sampler_count(uint32_t count);
    void set_combined_image_sampler_count(uint32_t count);
    void set_sampled_image_count(uint32_t count);
    void set_storage_image_count(uint32_t count);
    void set_uniform_texel_buffer_count(uint32_t count);
    void set_storage_texel_buffer_count(uint32_t count);
    void set_uniform_buffer_count(uint32_t count);
    void set_storage_buffer_count(uint32_t count);
    void set_dynamic_uniform_buffer_count(uint32_t count);
    void set_dynamic_storage_buffer_count(uint32_t count);
    void set_input_attachment_count(uint32_t count);

    std::vector<DescriptorSet*> allocate(const std::vector<DescriptorSetLayout*>& layouts);

private:
    uint32_t _max_sets{ 0 };
    std::vector<DescriptorSet*> _sets;

    uint32_t _sampler_count{ 0 };
    uint32_t _combined_image_sampler_count{ 0 };
    uint32_t _sampled_image_count{ 0 };
    uint32_t _storage_image_count{ 0 };
    uint32_t _uniform_texel_buffer_count{ 0 };
    uint32_t _storage_texel_buffer_count{ 0 };
    uint32_t _uniform_buffer_count{ 0 };
    uint32_t _storage_buffer_count{ 0 };
    uint32_t _dynamic_uniform_buffer_count{ 0 };
    uint32_t _dynamic_storage_buffer_count{ 0 };
    uint32_t _input_attachment_count{ 0 };

    VkDescriptorPool _vk_pool{ VK_NULL_HANDLE };
};

}

#endif
