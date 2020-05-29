#pragma once

#ifndef REND_WINDOW_CONTEXT_H
#define REND_WINDOW_CONTEXT_H

namespace rend
{
	class Window;

	class WindowContext
	{
	public:
		static WindowContext& instance(void);

		Window* window(void);
		void    set_window(Window& window);

	private:
		WindowContext(void) = default;
		~WindowContext(void);

	private:
		Window* _window{ nullptr };
	};
}

#endif
