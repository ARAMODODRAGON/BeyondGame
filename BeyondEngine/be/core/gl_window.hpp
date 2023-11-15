#ifndef BOE_GL_WINDOW_HPP
#define BOE_GL_WINDOW_HPP

struct SDL_Window;

namespace be {

	class gl_window final {
	public:

		gl_window();
		~gl_window();

		void poll_events();

	private:
		SDL_Window* m_window;
		void* m_glContext;


	};

}

#endif // !BOE_GL_WINDOW_HPP