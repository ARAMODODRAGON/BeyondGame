#ifndef BE_GAME_HPP
#define BE_GAME_HPP
#include "common.hpp"
#include "core\window.hpp"

namespace be {

	// a basic game class that will initialize the game
	// then calls 'virtual mainloop' to have the user handle the loop
	class game {
	public:

		game();

		virtual ~game() = 0;

		// starts the main loop
		void run(const glm::uvec2& screenSize, const std::string& titlebar);

	protected:

		// the main loop of the game
		virtual void mainloop(window* w) = 0;

	private:
	};

}

#endif // !BE_GAME_HPP