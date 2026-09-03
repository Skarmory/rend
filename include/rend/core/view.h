#ifndef REND_CORE_VIEW_H
#define REND_CORE_VIEW_H

#include "rend/rend_api.h"
#include "rend/core/descriptor_set_binding.h"
#include "rend/core/gpu_resource.h"
#include "rend/core/rend_object.h"

#include <vector>

namespace rend
{

class DescriptorSetLayout;
class DescriptorSet;

struct ViewInfo
{
    DescriptorSetLayout* descriptor_set_layout{ nullptr };
    std::vector<DescriptorSetBinding> descriptors;
};

class REND_API View : public GPUResource, public RendObject
{
    public:
        View(const std::string& name, const ViewInfo& info);
        virtual ~View(void) = default;

        ViewInfo& get_view_info(void);
        DescriptorSet& get_descriptor_set(void);

    private:
        ViewInfo _info;
        DescriptorSet* _descriptor_set{ nullptr };
};

}

#endif
