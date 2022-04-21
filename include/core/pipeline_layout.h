#ifndef REND_PIPELINE_LAYOUT_H
#define REND_PIPELINE_LAYOUT_H

#include "rend_defs.h"

namespace rend
{

class PipelineLayout
{
public:
    explicit PipelineLayout(const PipelineLayoutInfo& info);
    ~PipelineLayout(void);
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) = delete;

    PipelineLayoutHandle handle(void) const;

private:
    PipelineLayoutHandle _handle{ NULL_HANDLE };
    PipelineLayoutInfo   _info{};
};

}

#endif
