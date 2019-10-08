#include "window_base.h"

using namespace rend;

WindowBase::WindowBase(void)
    : _width(0),
      _height(0)
{
}

WindowBase::~WindowBase(void)
{
}

void WindowBase::resize(uint32_t width, uint32_t height)
{
    _width  = width;
    _height = height;
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
