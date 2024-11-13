#include "core/pipeline_layout.h"

#include "core/device_context.h"
#include "core/rend_service.h"

using namespace rend;

PipelineLayout::PipelineLayout(const std::string& name)
    :
        GPUResource(name)
{
}
