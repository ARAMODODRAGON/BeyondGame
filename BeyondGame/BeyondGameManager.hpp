#ifndef BEYONDGAMEMANAGER_HPP
#define BEYONDGAMEMANAGER_HPP
#include <be\game_manager.hpp>

class BeyondGameManager final : public be::game_manager {
public:

	BeyondGameManager();
	~BeyondGameManager();

protected:

	// called before the main game loop starts
	void on_init() final;

	// called every frame
	void on_step() final;

	// called every frame before all draw calls
	void on_draw() final;

	// called after the main game loop ends
	void on_exit() final;

private:

};

#endif // !BEYONDGAMEMANAGER_HPP