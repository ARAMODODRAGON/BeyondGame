#include <SDL.h>
#include "be\game.hpp"
#include "be\core\graphics\sprite_batch.hpp"
#include <glm\gtc\matrix_transform.hpp>

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

		// create some objects
		constexpr float objSize = 10.0f;
		be::bounds2d standard(-objSize, -objSize, objSize, objSize);
		Object o;
		constexpr size_t objCount = 100000;
		m_objects.reserve(objCount);
		for (size_t i = 0; i < objCount; i++) {
			o.position.x = random_between(CAM_WIDTH * -0.5f, CAM_WIDTH * 0.5f);
			o.position.y = random_between(CAM_HEIGHT * -0.5f, CAM_HEIGHT * 0.5f);
			o.bounds = standard;
			o.color.a = 1.0f;
			o.color.r = random_between(0.1f, 1.0f);
			o.color.g = random_between(0.1f, 1.0f);
			o.color.b = random_between(0.1f, 1.0f);
			m_objects.push_back(o);
		}

		m_batch->clear();

		// fill batch
		for (auto& obj : m_objects) {
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(obj.position, 0.0f));
			m_batch->push(
				obj.bounds,
				obj.color,
				nullptr,
				nullptr,
				&transform
			);
		}

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

			// begin draw
			// draw batch
			m_batch->draw(m_view);

			// end frame
			w->swap_buffers();
			time.wait_for_frame();
		}
	}

private:
	be::sprite_batch* m_batch;
	glm::mat4 m_view;
	std::vector<Object> m_objects;
};

int main(int argc, char** argv) {

	be::run_game<BeyondGame>(glm::uvec2(1280, 720), "Demo");

	return 0;
}