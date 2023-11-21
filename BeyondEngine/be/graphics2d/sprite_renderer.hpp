#ifndef BE_SPRITE_RENDERER_HPP
#define BE_SPRITE_RENDERER_HPP
#include "../object2d.hpp"
#include "../content/texture2d.hpp"
#include "../content/shader.hpp"

namespace be {

	class sprite_renderer final : public object2d {
	public:

		sprite_renderer();
		~sprite_renderer();


	private:
		shared<texture2d> m_texture2d;
		shared<shader> m_shader;
		friend class renderer2d;
	};

}

#endif // !BE_SPRITE_RENDERER_HPP