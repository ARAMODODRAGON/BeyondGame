#ifndef BOE_GL_WINDOW_HPP
#define BOE_GL_WINDOW_HPP
#include "../common.hpp"

struct SDL_Window;

namespace be {

	class gl_window final {
	public:

		gl_window();
		~gl_window();

		void clear_screen(const glm::vec4& color);

		void swap_buffers();

		void poll_events();

	private:
		SDL_Window* m_window;
		void* m_glContext;
	};

}

#endif // !BOE_GL_WINDOW_HPP