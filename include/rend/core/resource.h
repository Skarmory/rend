#ifndef REND_RESOURCE_H
#define REND_RESOURCE_H

#include "rend/rend_api.h"
#include "rend_deFs.h"

namespace rend
{

class REND_API Resource
{
public:
    StatusCode create_resource(void)
    {
        _initialised = true;
        return StatusCode::SUCCESS;
    }

    StatusCode destroy_resource(void)
    {
        _initialised = false;
        return StatusCode::SUCCESS;
    }

    bool initialised(void) const
    {
        return _initialised;
    }

private:
    bool _initialised { false };

};

}

#endif
