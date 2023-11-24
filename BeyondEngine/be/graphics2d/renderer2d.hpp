#ifndef BE_RENDERER2D__HPP
#define BE_RENDERER2D__HPP
#include "../object.hpp"
#include "../core/content/texture2d.hpp"
#include "../core/content/shader.hpp"

namespace be {

	class sprite_renderer;

	// singleton renderer object
	class renderer2d final : public object {
	public:

		// singleton function
		static renderer2d* get();

		renderer2d();
		~renderer2d();



	private:
		static renderer2d* s_instance;

		struct texture2d_group {
			shared<texture2d> usingtexture;
			std::list<sprite_renderer> renderers;
		};

		struct shader_group {
			shared<shader> usingShader;
			std::list<texture2d_group> textureGroups;
		};

	};

}

#endif // !BE_RENDERER_2D_HPP