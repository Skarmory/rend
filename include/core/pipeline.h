#ifndef REND_PIPELINE_H
#define REND_PIPELINE_H

#include "rend_defs.h"

namespace rend
{

class Pipeline
{
public:
    explicit Pipeline(const PipelineInfo& info);
    ~Pipeline(void);
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    PipelineHandle handle(void) const;

private:
    PipelineHandle _handle{ NULL_HANDLE };
    PipelineInfo _info{};
};

}

#endif
