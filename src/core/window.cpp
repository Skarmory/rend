#include "window.h"

#include <cstdio>
#include <cstring>

using namespace rend;

bool Window::create_window(uint32_t width, uint32_t height, const char* title)
{
    if(create_window_api(width, height, title) != StatusCode::SUCCESS)
    {
        return false;
    }

    resize(width, height);
    set_title(_title);

    return true;
}

bool Window::create_window(void)
{
    return create_window(_width, _height, _title);
}
