#include "core/view.h"

#include <sstream>
#include <string>

#include "core/descriptor_set.h"
#include "core/renderer.h"

using namespace rend;

View::View(const std::string& name, const ViewInfo& info)
    :
        GPUResource(name),
        _info(info)
{
    auto& rr = Renderer::get_instance();

    std::stringstream ss;
    ss << name << " descriptor set";

    _descriptor_set = rr.create_descriptor_set(ss.str(), *_info.descriptor_set_layout);

    for(auto& descriptor : _info.descriptors)
    {
        _descriptor_set->bind_resource(descriptor);
    }

    _descriptor_set->write_bindings();
}

ViewInfo& View::get_view_info(void)
{
    return _info;
}

DescriptorSet& View::get_descriptor_set(void)
{
    return *_descriptor_set;
}
