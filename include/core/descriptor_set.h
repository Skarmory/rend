#ifndef REND_DESCRIPTOR_SET_H
#define REND_DESCRIPTOR_SET_H

#include "core/alloc/allocator.h"
#include "core/containers/data_array.h"
#include "core/descriptor_set_binding.h"
#include "core/rend_defs.h"

#include <vector>

namespace rend
{

struct DescriptorSetInfo
{
    DescriptorSetLayoutHandle layout_handle{ NULL_HANDLE };
    uint32_t                  set_number{ 0 };
};

class DescriptorSet
{
    friend class Allocator<DescriptorSet>;

public:
    DescriptorSet(const DescriptorSet&)            = delete;
    DescriptorSet(DescriptorSet&&)                 = delete;
    DescriptorSet& operator=(const DescriptorSet&) = delete;
    DescriptorSet& operator=(DescriptorSet&&)      = delete;

    void add_uniform_buffer_binding(uint32_t slot, BufferHandle handle);
    void add_texture_binding(uint32_t slot, TextureHandle handle);
    void write_bindings(void);

    DescriptorSetHandle handle(void) const;
    uint32_t            set_number(void) const;

private:
    DescriptorSet(DescriptorPoolHandle pool_h, const DescriptorSetInfo& info);
    ~DescriptorSet(void);

private:
    DescriptorSetHandle               _handle{ NULL_HANDLE };
    std::vector<DescriptorSetBinding> _bindings;
    uint32_t                          _set_number{ 0 };
};

}

#endif
