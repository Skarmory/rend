#pragma once

#ifndef REND_WINDOW_CONTEXT_H
#define REND_WINDOW_CONTEXT_H

namespace rend
{
	class Window;

	// TODO: Don't like this at all, but meh... it'll do for now
	class WindowContext
	{
	public:
		static WindowContext& instance(void);

		Window* window(void);
		void    set_window(Window* window);

	private:
		WindowContext(void) = default;
		~WindowContext(void);

	private:
		Window* _window{ nullptr };
	};
}

#endif
