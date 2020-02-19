#include "window_base.h"

using namespace rend;

void WindowBase::resize(uint32_t width, uint32_t height)
{
    _width  = width;
    _height = height;
}

void WindowBase::set_title(const char* title)
{
    snprintf(_title, C_TITLE_LENGTH_MAX, title);
}

uint32_t WindowBase::width(void) const
{
    return _width;
}

uint32_t WindowBase::height(void) const
{
    return _height;
}

const char* WindowBase::title(void) const
{
    return _title;
}
