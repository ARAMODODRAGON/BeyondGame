#ifndef BOE_GL_WINDOW_HPP
#define BOE_GL_WINDOW_HPP
#include "..\common.hpp"

struct SDL_Window;

namespace be {
	namespace core {

		class window final {
		public:

			window(const glm::uvec2& screenSize, const std::string& titlebar);
			~window();

			// should be called at the begining of every frame
			// returns if the game should continue running or not
			// updates input data accessable in be/core/input
			bool poll_events();

			// clears the screen to the given color
			void clear_screen(const glm::vec4& color);

			// swaps buffers
			// should be called at the end of the frame
			void swap_buffers();


		private:
			SDL_Window* m_window;
			void* m_glContext;
			glm::uvec2 m_screenSize;
		};

	}
}

#endif // !BOE_GL_WINDOW_HPP