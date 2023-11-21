#ifndef BE_GAME_MANAGER_HPP
#define BE_GAME_MANAGER_HPP
#include "object.hpp"
#include "core\game_handler.hpp"

namespace be {

	class game_manager : public object {
	public:

		game_manager() { }
		virtual ~game_manager() = 0 { }

	protected:

		// called before the main game loop starts
		virtual void on_init() { }

		// called every frame
		virtual void on_step() { }

		// called every frame before all draw calls
		virtual void on_draw() { }

		// called after the main game loop ends
		virtual void on_exit() { }

	private:
		friend game_handler;
	};

}

#endif // !BE_GAME_MANAGER_HPP