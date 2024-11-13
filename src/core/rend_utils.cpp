#include "core/rend_utils.h"

bool rend::is_depth_format(rend::Format format)
{
    return format == rend::Format::D24_S8;
}
