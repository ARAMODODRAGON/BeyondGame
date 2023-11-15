#include "game_context.hpp"

namespace be {

	unique<game_context> game_context::s_instance;

	game_context::game_context() 
		: m_isRunning(false), m_shouldQuit(false), m_window(nullptr) { }

	game_context* game_context::get() {
		if (!s_instance) s_instance.reset(new game_context());
		return s_instance.get();
	}

	void game_context::run() { 
		if (m_isRunning) {
			BE_WARNING("Calling game_context::run() when game is already running is invalid");
			return;
		}

		// initialize
		m_window = new gl_window();
		m_isRunning = true;
		
		// call event
		on_start();

		// start game loop
		while (m_isRunning && !m_shouldQuit) {

			m_window->poll_events();

			// call event
			on_update(0.0f);


			m_window->swap_buffers();
			m_window->clear_screen(glm::vec4(0.f, 0.f, 0.f, 1.f));
		}
		m_isRunning = false;
	
		// call event
		on_exit();

		// clean up
		delete m_window; m_window = nullptr;
		m_shouldQuit = false;
	}

	bool game_context::is_running() const {
		return m_isRunning;
	}

	void game_context::quit() { 
		m_shouldQuit = true;
	}

	gl_window* game_context::get_window() const {
		return m_window;
	}


}