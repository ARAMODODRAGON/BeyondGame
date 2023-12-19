#include "spriterenderer.hpp"

namespace be {

	spriterenderer::spriterenderer() : m_layerIndex(0), m_renderer2d(nullptr) {}

	void spriterenderer::init() {
		object2d::init();
		m_renderer2d = get_instance<graphics2d::renderer2d>();
		m_renderer2d->__add_renderer(m_layerIndex, this);
	}

	spriterenderer::~spriterenderer() {
		m_renderer2d->__remove_renderer(m_layerIndex, this);
	}

	uint32 spriterenderer::get_layer_index() const {
		return m_layerIndex;
	}

	void spriterenderer::set_layer_index(uint32 layer) {
		if (m_layerIndex != layer) {
			m_renderer2d->__remove_renderer(m_layerIndex, this);
			m_layerIndex = layer;
			m_renderer2d->__add_renderer(m_layerIndex, this);
		}
	}

	void spriterenderer::set_sprite_bounds(const rect2& bounds) {
		m_spriteBounds = bounds;
	}

	void spriterenderer::load_texture(const std::string& file) {
		// TODO:
	}

	void spriterenderer::set_texture_uvs(const rect2& uvs) {
		m_uvs = uvs;
	}

}