#ifndef REND_CORE_DESCRIPTOR_SET_H
#define REND_CORE_DESCRIPTOR_SET_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_update_rate.h"
#include "rend/core/descriptor_set_binding.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_defs.h"
#include "rend/core/rend_object.h"

#include <vector>

namespace rend
{

    class DescriptorSetLayout;



    class DescriptorSet : public GPUResource, public RendObject
    {
    public /* methods */:

        DescriptorSet(const std::string& name, const DescriptorSetLayout& layout);
        virtual ~DescriptorSet(void) = default;
        DescriptorSet(const DescriptorSet&)            = delete;
        DescriptorSet(DescriptorSet&&)                 = delete;
        DescriptorSet& operator=(const DescriptorSet&) = delete;
        DescriptorSet& operator=(DescriptorSet&&)      = delete;

        DescriptorUpdateRate get_update_rate(void) const;
        const std::vector<DescriptorSetBinding>& get_bindings(void) const;

        void bind_resource(const DescriptorSetBinding& descriptor);
        virtual void write_bindings(void) const = 0;

    private /*variables*/:
        const DescriptorSetLayout&        _layout;
        std::vector<DescriptorSetBinding> _bindings;
    };

}

#endif
