#include <SDL.h>
#include "be\core\game.hpp"
#include "be\object_registry.hpp"
#include "be\objects\renderer2d.hpp"
#include "be\objects\camera2d.hpp"
#include "be\objects\spriterenderer.hpp"
#include <lua.hpp>
#include <unordered_set>

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

class NPC final : public be::spriterenderer {
public:

	NPC() : m_speed(1.7f) {}
	~NPC() {}

	void init() override {
		be::spriterenderer::init();

		// init sprite properties
		set_sprite_bounds(be::rect2(glm::vec2{-10.0f, -10.0f}, glm::vec2{10.0f, 10.0f}));

		// setup listeners
		listen<&NPC::step>("step");
	}

private:

	void step() {
		glm::vec2 direction = {0.0f, 0.0f};

		const bool left = be::input::button("arrow_left");
		const bool right = be::input::button("arrow_right");
		const bool up = be::input::button("arrow_up");
		const bool down = be::input::button("arrow_down");

		if (left != right) {
			if (left)	direction.x = -1.0f;
			if (right)	direction.x = 1.0f;
		}

		if (down != up) {
			if (down)	direction.y = -1.0f;
			if (up)		direction.y = 1.0f;
		}

		// we dont want to normalize if 0
		if (glm::length2(direction) > 0.000001f)
			direction = glm::normalize(direction);

		glm::vec2 pos = get_position();
		set_position(pos + (direction * m_speed));
	}

	float m_speed;
};

class BeyondGame final : public be::core::igame {
public:

	BeyondGame() : m_registry(nullptr), m_renderer2d(nullptr) {
		// batch & view
		m_registry = new be::object_registry();
		m_renderer2d = m_registry->get_instance<be::graphics2d::renderer2d>();

		// create objects
		auto* camera = m_registry->create<be::camera2d>();
		camera->set_size({CAM_WIDTH, CAM_HEIGHT});

		NPC* npc = m_registry->create<NPC>();
	}

	~BeyondGame() {
		delete m_registry; m_registry = nullptr;
	}

	void mainloop(be::core::window* w) override {
		// timer
		be::core::timer time;
		time.set_fps_target(60);
		float t0 = 0.0f;

		// main loop
		while (w->poll_events()) {
			time.begin_frame();
			w->clear_screen(BE_COLOR_BLACK);

			// step
			m_registry->message("step");

			// draw
			m_renderer2d->draw();

			// end frame
			w->swap_buffers();
			time.wait_for_frame();
		}

	}

private:

	be::object_registry* m_registry;
	be::graphics2d::renderer2d* m_renderer2d;
};

int main(int argc, char** argv) {

	be::core::run_game<BeyondGame>(glm::uvec2(1280, 720), "Demo");

	return 0;
}