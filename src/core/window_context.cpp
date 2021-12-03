#include "core/window_context.h"

#include "core/window.h"

using namespace rend;

WindowContext::~WindowContext(void)
{
	delete _window;
}

WindowContext& WindowContext::instance(void)
{
	static WindowContext _instance;

	return _instance;
}

Window* WindowContext::window(void)
{
	return _window;
}

void WindowContext::set_window(Window* window)
{
	_window = window;
}
