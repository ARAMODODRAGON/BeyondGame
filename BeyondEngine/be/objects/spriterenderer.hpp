#ifndef BE_SPRITERENDERER_HPP
#define BE_SPRITERENDERER_HPP
#include "object2d.hpp"
#include "renderer2d.hpp"

namespace be {

	// represents a sprite to be rendered
	// can have a layer index which determines the order in which sprites are drawn
	class spriterenderer : public object2d {
	protected:
		void init() override;
	public:

		spriterenderer();

		virtual ~spriterenderer();

		uint32 get_layer_index() const;

		void set_layer_index(uint32 layer);

		void set_sprite_bounds(const rect2& bounds);

		void load_texture(const std::string& file);

		void set_texture_uvs(const rect2& uvs);

	private:
		int32 m_layerIndex;

		content::texture2d m_texture;
		rect2 m_spriteBounds;
		rect2 m_uvs;

		graphics2d::renderer2d* m_renderer2d;
		friend graphics2d::renderer2d;
	};

}

#endif // !BE_SPRITERENDERER_HPP