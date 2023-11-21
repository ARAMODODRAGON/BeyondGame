#include "object2d.hpp"

namespace be {

	object2d::object2d()
		: m_position(0.0f, 0.0f)
		, m_rotation(0.0f)
		, m_scale(1.0f, 1.0f)
		, m_parent2d(nullptr)
		, m_checkonce(false) { }

	object2d::~object2d() { }

	const glm::vec2& object2d::get_position() const {
		if (auto* p = _get_parent2d()) 
			return m_position + p->get_position();
		else
			return m_position;
	}

	void object2d::set_position(const glm::vec2 position) { 
		if (auto* p = _get_parent2d()) 
			m_position = position - p->get_position();
		else
			m_position = position;
	}

	const glm::vec2& object2d::get_local_position() const {
		return m_position;
	}

	void object2d::set_local_position(const glm::vec2 position) { 
		m_position = position;
	}

	float object2d::get_rotation() const {
		if (auto* p = _get_parent2d()) 
			return m_rotation + p->get_rotation();
		else
			return m_rotation;
	}

	void object2d::set_rotation(float rotation) { 
		if (auto* p = _get_parent2d()) 
			m_rotation = rotation - p->get_rotation();
		else
			m_rotation = rotation;
	}

	float object2d::get_local_rotation() const {
		return m_rotation;
	}

	void object2d::set_local_rotation(float rotation) { 
		m_rotation = rotation;
	}

	const glm::vec2 object2d::get_scale() const {
		if (auto* p = _get_parent2d()) 
			return m_scale * p->get_scale();
		else
			return m_scale;
	}

	void object2d::set_scale(const glm::vec2& scale) { 
		if (auto* p = _get_parent2d()) 
			m_scale = scale / p->get_scale();
		else
			m_scale = scale;
	}

	const glm::vec2 object2d::get_local_scale() const {
		return m_scale;
	}

	void object2d::set_local_scale(const glm::vec2& scale) { 
		m_scale = scale;
	}

	object2d* object2d::_get_parent2d() const {
		if (!m_checkonce) {
			m_checkonce = true;
			m_parent2d = dynamic_cast<object2d*>(get_parent());
		}
		return m_parent2d;
	}

}