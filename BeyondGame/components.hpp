#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP
#include <be\common.hpp>
#include <be\core\canvas.hpp>
#include <entt\entt.hpp>

struct actor_component final : public be::transform2d {
	bool is_active : 1 = true;
	bool recalculate_mat4 : 1 = true;
	glm::mat4 transform_matrix = glm::mat4(1.0f);
};

struct sprite_component final {
	be::rect2 shape = {glm::vec2(-0.5f), glm::vec2(0.5f)};
	glm::vec4 color = BE_COLOR_WHITE;
};


#endif // !COMPONENTS_HPP