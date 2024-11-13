#ifndef REND_CORE_DESCRIPTOR_SET_H
#define REND_CORE_DESCRIPTOR_SET_H

#include "core/descriptor_frequency.h"
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

class DescriptorSet : public GPUResource, public RendObject
{
public /*methods*/:
    DescriptorSet(const std::string& name, const DescriptorSetLayout& info);
    virtual ~DescriptorSet(void) = default;
    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    DescriptorFrequency                      get_index(void) const;
    const std::vector<DescriptorSetBinding>& get_bindings(void) const;

    void bind_resource(const DescriptorSetBinding& descriptor);
    virtual void write_bindings(void) const = 0;

private /*variables*/:
    const DescriptorSetLayout&        _layout;
    std::vector<DescriptorSetBinding> _bindings;
};

}

#endif
