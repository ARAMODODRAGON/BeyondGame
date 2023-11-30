#include <SDL.h>
#include "be\game.hpp"
#include "be\core\graphics\sprite_batch.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <lua.hpp>

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

struct Object {
	be::bounds2d bounds;
	glm::vec2 position;
	glm::vec4 color;
};

static float random_between(float min, float max) {
	return (max - min) * (float(rand()) / float(RAND_MAX)) + min;
}

class BeyondGame final : public be::igame {
public:

	BeyondGame() : m_batch(nullptr), m_view(1.0f) {
		// batch & view
		m_batch = new be::sprite_batch();
		m_view = glm::ortho(CAM_WIDTH * -0.5f, CAM_WIDTH * 0.5f, CAM_HEIGHT * -0.5f, CAM_HEIGHT * 0.5f);

	}

	~BeyondGame() {
		if (m_batch) delete m_batch; m_batch = nullptr;
	}

	void mainloop(be::window* w) override {
		// initialize

		// timer
		be::timer time;
		time.set_fps_target(60);
		float t0 = 0.0f;

		// main loop
		while (w->poll_events()) {
			time.begin_frame();
			w->clear_screen(BE_COLOR_BLACK);

			// update 
			t0 += time.get();
			if (t0 > 1.0f) {
				t0 -= 1.0f;
				BE_LOG("current framerate: " + std::to_string(1.0f / time.get()));
			}
			step(time.get());

			// begin draw
			m_batch->clear();

			// push objects
			draw();

			// draw batch
			m_batch->draw(m_view);

			// end frame
			w->swap_buffers();
			time.wait_for_frame();
		}
	}

	void step(float delta) {

	}

	void draw() {

	}

private:
	be::sprite_batch* m_batch;
	glm::mat4 m_view;
	std::vector<Object> m_objects;
};

int main(int argc, char** argv) {

	//be::run_game<BeyondGame>(glm::uvec2(1280, 720), "Demo");

	lua_State* L = luaL_newstate(); 
	luaL_dostring(L, " x = 42");
	// Get global variable called 'x' and push it to the start of the Lua stack.
	lua_getglobal(L, "x");
	lua_Number x = lua_tonumber(L, 1); // Get the value at the top of the stack.
	BE_LOG("Lua says " + std::to_string(x));
	lua_close(L); // Deallocate the LUA state.

	return 0;
}