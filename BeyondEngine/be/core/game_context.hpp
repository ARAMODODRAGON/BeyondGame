#ifndef BE_GAME_CONTEXT_HPP
#define BE_GAME_CONTEXT_HPP
#include "../common.hpp"
#include "gl_window.hpp"
#include "../objects/object.hpp"

namespace be {

	class game_context final {
	public:

		// singleton method
		static game_context* get();

		// self contained initialization and game loop 
		// can only be called once within its runtime
		void run();

		// checks if the game is running
		bool is_running() const;

		// quits the game once it reaches the end of this step
		void quit();

		// gets the current gl_window
		gl_window* get_window() const;

		// event called after initialization before the first step
		event<void> on_start;

		// event called upon exiting the game before the deletion of anything else
		// void on_exit();
		event<void> on_exit;
		
		// event called at the begining of each step
		// void on_update(float deltatime)
		event<void, float> on_update;

		// returns the root object of this game
		object* get_root();

	private:
		game_context();
		static unique<game_context> s_instance;
		
		bool m_isRunning : 1;
		bool m_shouldQuit : 1;
		gl_window* m_window;
		object_factory m_factory;
	};

}

#endif // !BE_GAME_CONTEXT_HPP