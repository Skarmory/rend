#include "rend/core/pipeline_layout.h"

#include "rend/core/device_context.h"
#include "rend/core/rend_service.h"

using namespace rend;

PipelineLayout::PipelineLayout(const std::string& name)
    :
        GPUResource(name)
{
}
