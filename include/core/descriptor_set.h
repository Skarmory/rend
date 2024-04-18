#ifndef REND_CORE_DESCRIPTOR_SET_H
#define REND_CORE_DESCRIPTOR_SET_H

#include "core/descriptor_set_binding.h"
#include "core/gpu_resource.h"
#include "core/rend_defs.h"
#include "core/rend_object.h"

#include <vector>

namespace rend
{

class DescriptorSetLayout;
class GPUBuffer;
class GPUTexture;

struct DescriptorSetInfo
{
    const DescriptorSetLayout* layout{ nullptr };
    uint32_t                   set{ 0 };
};

class DescriptorSet : public GPUResource, public RendObject
{
public:
    DescriptorSet(const std::string& name, const DescriptorSetInfo& info, RendHandle rend_handle);
    virtual ~DescriptorSet(void) = default;
    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    void add_uniform_buffer_binding(uint32_t slot, GPUBuffer* buffer);
    void add_texture_binding(uint32_t slot, GPUTexture* texture);

    uint32_t                                 set(void) const;
    const std::vector<DescriptorSetBinding>& bindings(void) const;

private:
    uint32_t                          _set{ 0 };
    const DescriptorSetLayout*        _layout{ nullptr };
    std::vector<DescriptorSetBinding> _bindings;
};

}

#endif
