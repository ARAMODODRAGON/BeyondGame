#include <SDL.h>
#include "be\game.hpp"
#include "be\core\graphics\sprite_batch.hpp"
#include "be\core\scripting\lua_script.hpp"
#include <lua.hpp>

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

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
};

int main(int argc, char** argv) {

	//be::run_game<BeyondGame>(glm::uvec2(1280, 720), "Demo");
	be::lua_script* script0 = new be::lua_script("Scripts/Demo.lua");
	be::lua_script* script1 = new be::lua_script("Scripts/Demo.lua");

	auto* state = be::lua_script::get_state();

	lua_getglobal(state, script0->env());
	lua_pushnumber(state, 5);
	lua_setfield(state, -2, "value");
	lua_pop(state, 1);

	lua_getglobal(state, script1->env());
	lua_pushnumber(state, 10);
	lua_setfield(state, -2, "value");
	lua_pop(state, 1);

	script0->call("foo");
	script1->call("foo");

	delete script0;
	delete script1;
	return 0;
}