#include "window.hpp"
#include <SDL.h>
#include <stdio.h>
#include <glew\glew.h>
#include <stdexcept>
#include "input.hpp"

namespace be {

	window::window(const glm::uvec2& screenSize, const std::string& titlebar)
		: m_window(nullptr)
		, m_glContext(nullptr)
		, m_screenSize(screenSize) {

		// initialize
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

		// set some attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// this matches to the refresh rate of the display
		SDL_GL_SetSwapInterval(1);

		// experimental? something something, yea!
		glewExperimental = GL_TRUE;


		// create
		m_window = SDL_CreateWindow(
			titlebar.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			m_screenSize.x, m_screenSize.y,
			SDL_WINDOW_OPENGL
		);

		// check if window was created
		if (!m_window) {
			BE_FATAL_ERROR("Failed to create window");
			throw std::runtime_error("Failed to create window");
		}

		// create context
		m_glContext = SDL_GL_CreateContext(m_window);

		// setup
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		// check if it was created
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			BE_FATAL_ERROR("Failed to initialize GLEW\n");
			throw std::runtime_error("Failed to initialize GLEW");
		}
	}

	window::~window() {
		// close out
		SDL_DestroyWindow(m_window);
		m_window = nullptr;

		SDL_Quit();
	}

	bool window::poll_events() {
		bool shouldKeepRunning = true;

		keyboard::__clear_changed_bits();
		mouse::__clear_changed_bits();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					shouldKeepRunning = false;
					break;
				case SDL_KEYDOWN: case SDL_KEYUP: // handle key 
				{
					// no need to check the key if it's repeating
					if (e.key.repeat) break;

					// letter on the keyboard
					if (e.key.keysym.sym > 96 && e.key.keysym.sym < 123)
						keyboard::__set(static_cast<keycode>(e.key.keysym.sym - 97), e.key.state);
					// number on keyboard
					else if (e.key.keysym.sym > 47 && e.key.keysym.sym < 58)
						keyboard::__set(static_cast<keycode>(e.key.keysym.sym - 48 + 26), e.key.state);
					// other
					else {
						switch (e.key.keysym.scancode) {
							case SDL_SCANCODE_TAB:       keyboard::__set(keycode::tab, e.key.state);         break;
							case SDL_SCANCODE_RETURN:	 keyboard::__set(keycode::retur, e.key.state);       break;
							case SDL_SCANCODE_BACKSPACE: keyboard::__set(keycode::backspace, e.key.state);   break;
							case SDL_SCANCODE_SPACE:     keyboard::__set(keycode::space, e.key.state);       break;
							case SDL_SCANCODE_ESCAPE:    keyboard::__set(keycode::escape, e.key.state);		 break;
							case SDL_SCANCODE_UP:        keyboard::__set(keycode::arrow_up, e.key.state);    break;
							case SDL_SCANCODE_DOWN:      keyboard::__set(keycode::arrow_down, e.key.state);  break;
							case SDL_SCANCODE_LEFT:      keyboard::__set(keycode::arrow_left, e.key.state);  break;
							case SDL_SCANCODE_RIGHT:     keyboard::__set(keycode::arrow_right, e.key.state); break;
							default: break;
						}
					}
					break; // SDL_KEYDOWN || SDL_KEYUP
				}
				case SDL_MOUSEMOTION:								// the mouse position
				{
					const glm::vec2 halfscreen(glm::vec2(m_screenSize) * 0.5f);
					glm::vec2 position(e.motion.x, e.motion.y);
					position -= halfscreen;
					position /= halfscreen;
					position.y = -position.y;
					mouse::__set_position(position);
					break;
				}
				case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:	// mouse buttons
				{
					switch (e.button.button) {
						case SDL_BUTTON_LEFT:	mouse::__set_button(0, e.button.state == SDL_PRESSED); break;
						case SDL_BUTTON_MIDDLE: mouse::__set_button(1, e.button.state == SDL_PRESSED); break;
						case SDL_BUTTON_RIGHT:	mouse::__set_button(2, e.button.state == SDL_PRESSED); break;
						case SDL_BUTTON_X1:		mouse::__set_button(3, e.button.state == SDL_PRESSED); break;
						case SDL_BUTTON_X2:		mouse::__set_button(4, e.button.state == SDL_PRESSED); break;
						default:break;
					}
					break;
				}
				case SDL_MOUSEWHEEL:								// mouse scroll wheel
				{
					mouse::__set_scroll(static_cast<float>(e.wheel.y)
						* (e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1.0f : -1.0f));
					break;
				}
				default: break;
			}
		}
		return shouldKeepRunning;
	}

	void window::clear_screen(const glm::vec4& color) {
		// clear screen
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void window::swap_buffers() {
		// swap buffers
		SDL_GL_SwapWindow(m_window);
	}

}