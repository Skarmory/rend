#ifndef REND_WINDOW_BASE_H
#define REND_WINDOW_BASE_H

#include <cstdint>

namespace rend
{

class WindowBase
{
public:
    WindowBase(void);
    ~WindowBase(void);

    WindowBase(const WindowBase&)            = delete;
    WindowBase(WindowBase&&)                 = delete;
    WindowBase& operator=(const WindowBase&) = delete;
    WindowBase& operator=(WindowBase&&)      = delete;

    void resize(uint32_t width, uint32_t height);

    uint32_t    width(void) const;
    uint32_t    height(void) const;
    const char* title(void) const;

public:
    static const uint32_t TITLE_LENGTH_MAX = 64;

protected:
    uint32_t _width;
    uint32_t _height;
    char     _title[TITLE_LENGTH_MAX];
};

}

#endif
