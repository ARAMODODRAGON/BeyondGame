#ifndef BE_GAME_HANDLER_HPP
#define BE_GAME_HANDLER_HPP
#include "../common.hpp"
#include "../object.hpp"

namespace be {

	class window;
	class renderer2d;
	class game_manager;

	// a builder class that initializes the game
	// contains a callback for the game loop or a generic game loop built in
	// static
	class game_handler final {
	public:

		// static function to end the game
		static void quit();

		// initializes the game
		game_handler();

		// deinitializes the game
		~game_handler();

		// sets the game_manager object
		// can only be called once. otherwise will return nullptr
		template<std::derived_from<game_manager> T>
		T* create_game_manager();

		// initializes the 2d renderer
		void init_renderer2d();

		// runs the game
		// if the game loop callback is set then that function will be called 
		// if not set then a generic game loop is run instead
		void run();

		// gets the window object
		window* get_window();

	private:
		object* m_root;

		// children of m_root
		game_manager* m_game_manager; 
		window* m_window;
		renderer2d* m_renderer2d;

		// bools
		bool m_shouldQuit : 1;

		// static reference
		static game_handler* s_currentHandler;
	};

	template<std::derived_from<game_manager> T>
	inline T* game_handler::create_game_manager() {
		if (m_game_manager) {
			BE_ERROR("Cannot create new game_manager as there is already one that exists");
			return nullptr;
		}
		auto* gm = m_root->create_child<T>();
		m_game_manager = gm;
		return gm;
	}

}

#endif // !BE_GAME_HANDLER_HPP