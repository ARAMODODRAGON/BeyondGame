#ifndef BE_SPRITE_RENDERER_HPP
#define BE_SPRITE_RENDERER_HPP
#include "../object2d.hpp"
#include "../core/content/texture2d.hpp"
#include "../core/content/shader.hpp"

namespace be {

	// sprite renderer object
	// contains a texture and shader
	// contains a position, rotation, and scale which affect the sprite
	class sprite_renderer final : public object2d {
	public:

		sprite_renderer();
		~sprite_renderer();


	private:
		shared<texture2d> m_texture2d;
		shared<shader> m_shader;
		bool m_isEnabled : 1;
	};

}

#endif // !BE_SPRITE_RENDERER_HPP