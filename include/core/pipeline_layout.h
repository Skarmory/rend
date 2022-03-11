#ifndef REND_PIPELINE_LAYOUT_H
#define REND_PIPELINE_LAYOUT_H

#include "rend_defs.h"

namespace rend
{

class PipelineLayout
{
public:
    PipelineLayout(void) = default;
    ~PipelineLayout(void) = default;
    PipelineLayout(const PipelineLayout&) = delete;
    PipelineLayout(PipelineLayout&&) = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&) = delete;

    bool create(const PipelineLayoutInfo& info);
    void destroy(void);

    PipelineLayoutHandle handle(void) const;

private:
    PipelineLayoutHandle _handle;
};

}

#endif
