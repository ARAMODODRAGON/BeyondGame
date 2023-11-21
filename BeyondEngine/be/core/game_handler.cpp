#include "game_handler.hpp"
#include "window.hpp"
#include "../game_manager.hpp"

namespace be {

	game_handler* game_handler::s_currentHandler = nullptr;;

	void game_handler::quit() {
		if (s_currentHandler)
			s_currentHandler->m_shouldQuit = true;
	}

	game_handler::game_handler()
		: m_root(nullptr)
		, m_game_manager(nullptr)
		, m_window(nullptr)
		, m_shouldQuit(false) {

		// create the root
		m_root = new object();

		// create nesessary subobjects
		m_window = m_root->create_child<window>();
	}

	game_handler::~game_handler() {
		// deletes the root (deletes all subobjects)
		delete m_root; m_root = nullptr;
	}

	void game_handler::run() {
		s_currentHandler = this;
		m_shouldQuit = false;

		if (m_game_manager) m_game_manager->on_init();

		while (!m_shouldQuit) {

			// step
			m_window->poll_events();
			if (m_game_manager) m_game_manager->on_step();


			// draw
			if (m_game_manager) m_game_manager->on_draw();
			m_window->swap_buffers();
			m_window->clear_screen({ 0.0f, 0.0f, 0.0f, 1.0f }); // clear to black
		}

		if (m_game_manager) m_game_manager->on_exit();

		s_currentHandler = nullptr;
	}

	window* game_handler::get_window() {
		return nullptr;
	}

}