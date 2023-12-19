#include "camera2d.hpp"

namespace be {
	camera2d::camera2d() : m_viewSize(720.0f, 1280.0f), m_view(1.0f), m_isDirty(true), m_renderer2d(nullptr) {}

	void camera2d::init() {
		object2d::init();
		m_renderer2d = get_instance<graphics2d::renderer2d>();
		m_renderer2d->__add_camera2d(this);
	}

	camera2d::~camera2d() {
		m_renderer2d->__remove_camera2d(this);
	}

	void camera2d::set_bounds(const rect2& bounds) {
		m_viewSize = bounds.max - bounds.min;
		set_position(bounds.min + (m_viewSize * 0.5f));
	}

	void camera2d::set_size(const glm::vec2& size) {
		m_viewSize = size;
	}

	const glm::mat4& camera2d::get_view() const {
		if (m_isDirty) {
			m_isDirty = false;
			const auto& position = get_position();
			m_view = glm::ortho(
				m_viewSize.x * -0.5f,
				m_viewSize.x * 0.5f,
				m_viewSize.y * -0.5f,
				m_viewSize.y * 0.5f
			);
			m_view = m_view * glm::inverse(get_transform()); // force recalculation of transform if needed
		}
		return m_view;
	}

	bool camera2d::is_main() const {
		return m_renderer2d->get_main_camera() == this;
	}

	void camera2d::set_main() {
		m_renderer2d->__set_main_camera2d(this);
	}

}
