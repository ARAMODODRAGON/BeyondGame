#include "gl_window.hpp"
#include <SDL.h>
#include <stdio.h>
#include <GL\glew.h>
#include <stdexcept>

namespace be {

	gl_window::gl_window() : m_window(nullptr) {

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
			"Demo",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1280, 720,
			SDL_WINDOW_OPENGL
		);

		// check if window was created
		if (!m_window) {
			printf("Failed to create window");
			throw std::runtime_error("Failed to create window");
		}

		// create context
		m_glContext = SDL_GL_CreateContext(m_window);

		// setup
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		// check if it was created
		GLenum error = glewInit();
		if (error != GLEW_OK) {
			printf("Failed to initialize GLEW\n");
			throw std::runtime_error("Failed to initialize GLEW");
		}

		int err = glGetError();


	}

	gl_window::~gl_window() {
		// close out
		SDL_DestroyWindow(m_window);
		m_window = nullptr;

		SDL_Quit();

	}

	void gl_window::poll_events() {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					break;
				default: break;
			}
		}
	}

}