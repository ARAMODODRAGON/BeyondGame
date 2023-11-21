#ifndef BOE_GL_WINDOW_HPP
#define BOE_GL_WINDOW_HPP
#include "../common.hpp"
#include "../object.hpp"

struct SDL_Window;

namespace be {

	class window final : public object {
	public:

		window();
		~window();

		void clear_screen(const glm::vec4& color);

		void swap_buffers();

		void poll_events();

	private:
		SDL_Window* m_window;
		void* m_glContext;
	};

}

#endif // !BOE_GL_WINDOW_HPP