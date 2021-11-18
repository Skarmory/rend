#ifndef REND_PIPELINE_H
#define REND_PIPELINE_H

#include "rend_defs.h"

namespace rend
{

class Pipeline
{
public:
    Pipeline(void) = default;
    ~Pipeline(void) = default;
    Pipeline(const Pipeline&) = delete;
    Pipeline(Pipeline&&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&) = delete;

    bool create(const PipelineInfo& info);
    void destroy(void);

    PipelineHandle handle(void) const;

private:
    PipelineHandle _handle;
};

}

#endif
