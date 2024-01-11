#ifndef DEMO_SCENE_HPP
#define DEMO_SCENE_HPP
#include "..\scene.hpp"
#include "..\components.hpp"

class demo_scene final : public scene {
	be::receiver receiver;
	entt::registry registry;
public:

	demo_scene();
	~demo_scene();

protected:

	void init() override;

	void step(float delta) override;

	void draw(be::canvas* canvas) override;

private:

	void draw_canvas(const glm::mat4& view);
	glm::vec2 m_offset;
	float m_zoom;
	be::canvas m_canvas;
};

#endif // !DEMO_SCENE_HPP