#ifndef BE_GAME_HPP
#define BE_GAME_HPP
#include "common.hpp"
#include "core\window.hpp"
#include "core\timer.hpp"

namespace be {

	// starts the main loop
	template<typename T>
	void run_game(const glm::uvec2& screenSize, const std::string& titlebar);


	// a basic game class that will initialize the game
	// then calls 'virtual mainloop' to have the user handle the loop
	class igame {
	public:

		virtual ~igame() = 0 {}

		// the main loop of the game
		virtual void mainloop(window* w) = 0;

	private:
	};

}


template<typename T>
void be::run_game(const glm::uvec2& screenSize, const std::string& titlebar) {
	// initialize game 
	unique<window> w(new window(screenSize, titlebar));
	unique<T> game(new T());

	// run
	game->mainloop(w.get());
}

#endif // !BE_GAME_HPP