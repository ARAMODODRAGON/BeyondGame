#ifndef GAME_HPP
#define GAME_HPP
#include <be\app.hpp>
#include <be\core\canvas.hpp>
#include <be\node.hpp>
#include "scene.hpp"

class game final : public be::game_node {
public:

	static inline be::signal<void, float> on_step;
	static inline be::signal<void, const glm::mat4&> on_draw;

	game();

	~game();

	void loadscene(size_t index);

protected:

	void ready() override;

	void step() override;

private:

	void game_step(float delta);

private:
	glm::mat4 m_view;
	be::core::timer m_time;
	scene* m_scene;
	size_t m_sceneIndex;
	be::unique<be::canvas> m_canvas;
};


#endif // !GAME_HPP