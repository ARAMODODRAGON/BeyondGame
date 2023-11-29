#include "game.hpp"

namespace be {

	game::game() {}

	game::~game() {}

	void game::run(const glm::uvec2& screenSize, const std::string& titlebar) {
		// initialize game 
		// rely on the stack order for destruction as nessisary
		unique<window> w(new window(screenSize, titlebar));

		// run
		mainloop(w.get());
	}


}