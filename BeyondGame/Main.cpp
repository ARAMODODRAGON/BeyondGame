#include <SDL.h>
#include "BeyondGameManager.hpp"

int main(int argc, char** argv) {

	// create
	be::game_handler* handler = new be::game_handler();
	handler->create_game_manager<BeyondGameManager>();

	// run
	handler->run();

	// delete
	delete handler; handler = nullptr;
	return 0;
}