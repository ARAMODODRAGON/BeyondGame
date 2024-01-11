#include "window.hpp"
#include <SDL.h>
#include <stdio.h>
#include <glew\glew.h>
#include <stdexcept>
#include "input.hpp"

namespace be {
	namespace core {

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
			SDL_GL_SetSwapInterval(0);

			// experimental? something something, yea!
			glewExperimental = GL_TRUE;

			// create
			m_window = SDL_CreateWindow(
				titlebar.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				m_screenSize.x, m_screenSize.y,
				SDL_WINDOW_OPENGL | SDL_RENDERER_PRESENTVSYNC
			);

			// check if window was created
			if (!m_window) {
				BE_FATAL_ERROR("Failed to create window");
				throw std::runtime_error("Failed to create window");
			}

			// create context
			m_glContext = SDL_GL_CreateContext(m_window);

			// setup
			//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

			// check if it was created
			GLenum error = glewInit();
			if (error != GLEW_OK) {
				BE_FATAL_ERROR("Failed to initialize GLEW\n");
				throw std::runtime_error("Failed to initialize GLEW");
			}

			// enable blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			

			// print graphics card and opengl version
			BE_LOG("Graphics card: " + std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER))));
			BE_LOG("OpenGL version: " + std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION))));

		}

		window::~window() {
			// close out
			SDL_DestroyWindow(m_window);
			m_window = nullptr;

			SDL_Quit();
		}

		bool window::poll_events() {
			bool shouldKeepRunning = true;

			input::__reset();

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
						if (e.key.keysym.scancode >= SDL_SCANCODE_A && e.key.keysym.scancode <= SDL_SCANCODE_Z)
							input::__set_button(e.key.keysym.scancode - SDL_SCANCODE_A, e.key.state);

						// number on keyboard
						else if (e.key.keysym.scancode >= SDL_SCANCODE_1 && e.key.keysym.scancode <= SDL_SCANCODE_0)
							input::__set_button(e.key.keysym.scancode - SDL_SCANCODE_1 + SDL_SCANCODE_Z - SDL_SCANCODE_A, e.key.state);
						
						// other
						else {
							switch (e.key.keysym.scancode) {
								case SDL_SCANCODE_TAB:       input::__set_button("key_tab", e.key.state);       break;
								case SDL_SCANCODE_RETURN:	 input::__set_button("key_return", e.key.state);    break;
								case SDL_SCANCODE_BACKSPACE: input::__set_button("key_backspace", e.key.state); break;
								case SDL_SCANCODE_SPACE:     input::__set_button("key_space", e.key.state);     break;
								case SDL_SCANCODE_ESCAPE:    input::__set_button("key_escape", e.key.state);	break;
								case SDL_SCANCODE_UP:        input::__set_button("arrow_up", e.key.state);		break;
								case SDL_SCANCODE_DOWN:      input::__set_button("arrow_down", e.key.state);	break;
								case SDL_SCANCODE_LEFT:      input::__set_button("arrow_left", e.key.state);	break;
								case SDL_SCANCODE_RIGHT:     input::__set_button("arrow_right", e.key.state);	break;
								default: break;
							}
						}
						break; // SDL_KEYDOWN || SDL_KEYUP
					}
					case SDL_MOUSEMOTION:								// the mouse position
					{
						//static glm::vec2 lastposition(0.0f);
						const glm::vec2 halfscreen(glm::vec2(m_screenSize) * 0.5f);
						glm::vec2 position(e.motion.x, e.motion.y);
						position -= halfscreen;
						position /= halfscreen;
						position.y = -position.y;
						input::__set_axis("mouse_x", position.x);
						input::__set_axis("mouse_y", position.y);
						//lastposition = position;
						break;
					}
					case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:	// mouse buttons
					{
						switch (e.button.button) {
							case SDL_BUTTON_LEFT:	input::__set_button("mouse_left", e.button.state == SDL_PRESSED); break;
							case SDL_BUTTON_MIDDLE: input::__set_button("mouse_middle", e.button.state == SDL_PRESSED); break;
							case SDL_BUTTON_RIGHT:	input::__set_button("mouse_right", e.button.state == SDL_PRESSED); break;
							case SDL_BUTTON_X1:		input::__set_button("mouse_four", e.button.state == SDL_PRESSED); break;
							case SDL_BUTTON_X2:		input::__set_button("mouse_five", e.button.state == SDL_PRESSED); break;
							default:break;
						}
						break;
					}
					case SDL_MOUSEWHEEL:								// mouse scroll wheel
					{
						input::__set_axis(
							"mouse_wheel",
							static_cast<float>(e.wheel.y) * (e.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1.0f : -1.0f)
						);
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
}