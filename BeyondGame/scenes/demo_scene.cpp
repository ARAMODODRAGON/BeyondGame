#include "demo_scene.hpp"
#include "..\game.hpp"

demo_scene::demo_scene() : m_offset(0.0f), m_zoom(1.0f) {
	set_name("Demo Scene");
	receiver.subscribe<&demo_scene::draw_canvas>(this, game::on_draw);
}

void demo_scene::init() {
	registry.ctx();

	constexpr float range = 10000.0f;

	// spawn 1000 sprites
	for (size_t i = 0; i < 100000; i++) {
		be::transform2d transform;
		transform.position.x = be::random.range(-range * 0.5f, range * 0.5f);
		transform.position.y = be::random.range(-range * 0.5f, range * 0.5f);

		glm::vec4 color = be::rgb(
			be::random.range(50, 255),
			be::random.range(50, 255),
			be::random.range(50, 255),
			127
		);

		transform.scale *= be::random.range(1.0f, 5.0f);

		be::rect2 shape = {glm::vec2(-0.5f), glm::vec2(0.5f)};

		constexpr float scale = 0.78f;
		be::rect2 part = shape;
		part.min *= scale;
		part.max *= scale;

		glm::mat4 transformation_matrix = be::get_mat4(transform);
		m_canvas.push(shape, color, nullptr, nullptr, &(transformation_matrix));
		m_canvas.push(part, glm::vec4(glm::vec3(0.5f), 1.0f), nullptr, nullptr, &(transformation_matrix));
	}
}

demo_scene::~demo_scene() {}

void demo_scene::step(float delta) {
	static glm::vec2 lastpos(0.0f);
	const glm::vec2 pos = {be::input::axis("mouse_x"), be::input::axis("mouse_y")};
	if (be::input::button("mouse_left")) {
		m_offset += -(lastpos - pos) * delta * 100000.0f;
	}
	lastpos = pos;
}

void demo_scene::draw(be::canvas* canvas) {}

void demo_scene::draw_canvas(const glm::mat4& view) {
	glm::mat4 output = glm::translate(glm::mat4(1.0f), glm::vec3(m_offset, 0.0f));
	m_canvas.draw(view * output);
}
