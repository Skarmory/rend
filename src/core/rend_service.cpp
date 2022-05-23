#include "core/rend_service.h"

using namespace rend;

// public

void RendService::provide(DeviceContext* context)
{
    _context = context;
}

void RendService::provide(Renderer* renderer)
{
    _renderer = renderer;
}

void RendService::provide(Window* window)
{
    _window = window;
}

DeviceContext* RendService::device_context(void)
{
    return _context;
}

Renderer* RendService::renderer(void)
{
    return _renderer;
}

Window* RendService::window(void)
{
    return _window;
}

DeviceContext* RendService::_context = nullptr;
Renderer* RendService::_renderer = nullptr;
Window* RendService::_window = nullptr;
