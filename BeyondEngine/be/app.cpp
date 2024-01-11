#include "app.hpp"

namespace be {

	game_node::game_node() : m_isRunning(false), m_shouldQuit(false) {}

	void be::game_node::quit() {
		m_shouldQuit = true;
	}

	core::window* game_node::window() {
		return m_window;
	}

	bool be::game_node::next_frame() {
		if (m_window == nullptr) return false;

		if (!m_window->poll_events()) {
			m_shouldQuit = true;
		}

		return m_isRunning && !m_shouldQuit;
	}

}