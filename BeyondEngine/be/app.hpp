#ifndef BE_GAME_HPP
#define BE_GAME_HPP
#include "common.hpp"
#include "core\window.hpp"
#include "core\timer.hpp"
#include "node.hpp"

namespace be {

	class game_node;

	// app class starts the game up and initializes anything required to run the game
	class app final {
	public:

		// starts the game loop and instances a game_node
		template<typename T> requires std::derived_from<T, game_node>
		static void run(const glm::uvec2& screenSize, cstring titlebar);

	};

	// a basic game class that will initialize the game
	// then calls 'virtual mainloop' to have the user handle the loop
	class game_node : public singleton_node {
	public:

		game_node();
		virtual ~game_node() = 0 {}

		// quits the game at the end of the frame
		void quit();

		core::window* window();

	protected:

		// called when all neccesary systems have been initialized
		virtual void ready() {}

		// called every frame
		virtual void step() = 0;

	private:
		bool m_isRunning : 1;
		bool m_shouldQuit : 1;
		core::window* m_window;

		bool next_frame();
		friend app;
	};

	template<typename T> requires std::derived_from<T, game_node>
	inline void app::run(const glm::uvec2& screenSize, cstring titlebar) {
		// initialize game 
		unique<core::window > win(new core::window(screenSize, titlebar));
		unique<node_tree> tree(new node_tree());

		// create game class
		game_node* game = tree->get_instance<T>();
		game->m_window = win.get();
		game->m_isRunning = true;

		// start
		game->ready();
		while (game->next_frame()) {
			game->step();
		}

		// exit
		tree.release(); // must be first
		win.release();
	}

}


#endif // !BE_GAME_HPP