#include <SDL.h>
#include <iostream>
#include <be\core\game_context.hpp>

static bool isRunning = true;

int main(int argc, char** argv) {

	be::game_context::get()->run();

	return 0;
}