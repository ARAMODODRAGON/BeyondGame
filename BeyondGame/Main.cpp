#include <SDL.h>
#include "be\game.hpp"

class BeyondGame final : public be::game {
public:

	BeyondGame() {

	}

	~BeyondGame() {


	}


protected:

	void mainloop(be::window* w) override {
		while (w->poll_events()) {
			w->clear_screen(BE_COLOR_BLACK);

			// update 
			

			// draw


			// end frame
			w->swap_buffers();
		}
	}

private:
	
};

int main(int argc, char** argv) {

	be::unique<BeyondGame> game(new BeyondGame());

	game->run(glm::uvec2(1280, 720), "Demo");

	return 0;
}