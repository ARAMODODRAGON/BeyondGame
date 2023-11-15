#include <SDL.h>
#include <iostream>
#include <be\core\gl_window.hpp>

static bool isRunning = true;

int main(int argc, char** argv) {

	// create window
	be::gl_window* window = new be::gl_window();

	// demo game loop
	while (isRunning) {
		
		window->poll_events();
	}
	
	delete window, window = nullptr;

	return 0;
}