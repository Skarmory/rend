#ifndef REND_WINDOW_BASE_H
#define REND_WINDOW_BASE_H

#include <cstdint>
#include <cstdio>

namespace rend
{

class WindowBase
{
public:
    WindowBase(void)                         = default;
    virtual ~WindowBase(void)                = default;
    WindowBase(const WindowBase&)            = delete;
    WindowBase(WindowBase&&)                 = delete;
    WindowBase& operator=(const WindowBase&) = delete;
    WindowBase& operator=(WindowBase&&)      = delete;

    virtual void resize(uint32_t width, uint32_t height) = 0;
    void set_title(const char* title);

    uint32_t    width(void) const;
    uint32_t    height(void) const;
    const char* title(void) const;

public:
    static const uint32_t C_TITLE_LENGTH_MAX = 64;

protected:
    uint32_t _width;
    uint32_t _height;
    char     _title[C_TITLE_LENGTH_MAX];
};

}

#endif
