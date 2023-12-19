#ifndef BE_OBJECT2D_HPP
#define BE_OBJECT2D_HPP
#include "..\object_registry.hpp"

namespace be {

	// simple representation of an object in 2d space
	// space is relative to the first ancestor that is also an object2d
	class object2d : public object {
		BE_OBJECT(object2d)
	protected:

		void init() override;
		//void on_reparent(object* oldParent) override;

	public:

		object2d();

		virtual ~object2d() {}

		// returns the first object3d that is an ancestor to this one
		// used to provide a world space get_transform() value
		object2d* get_object2d_ancestor();

		// returns a transform matrix in world space
		glm::mat4 get_transform() const;

		// returns the transform of this object in local space
		glm::mat4 get_local_transform() const;

		// returns if the transform is dirty
		bool transform_is_dirty() const;

		// returns the object space position
		const glm::vec2& get_position() const;

		// glm::vec2 get_world_position() const;

		// sets the object space position
		void set_position(const glm::vec2& position);

		// gets the object space scale
		const glm::vec2& get_scale() const;

		// glm::vec2 get_world_scale() const;

		// sets the object space scale
		void set_scale(const glm::vec2& scale);

		// gets the object space rotation
		float get_rotation() const;

		// float get_world_rotation() const;

		// sets the object space rotation
		void set_rotation(float angle);

	private:
		object2d* m_first2dancestor;
		glm::vec2 m_position;
		glm::vec2 m_scale;
		float m_angle;
		mutable glm::mat4 m_transform;
		mutable bool m_isDirty : 1;
	};


	/////// impl

	inline void object2d::init() {
		m_first2dancestor = get_ancestor<object2d>();
	}

	//inline void object2d::on_reparent(object* oldParent) {
	//	m_first2dancestor = get_ancestor<object2d>();
	//}

	inline object2d::object2d()
		: m_first2dancestor(nullptr)
		, m_position(0.0f)
		, m_scale(1.0f)
		, m_angle(0.0f)
		, m_transform(1.0f)
		, m_isDirty(true) {}

	inline object2d* object2d::get_object2d_ancestor() {
		return m_first2dancestor;
	}

	inline glm::mat4 object2d::get_transform() const {
		if (m_first2dancestor)
			return get_local_transform() * m_first2dancestor->get_transform();
		else
			return get_local_transform();
	}

	inline glm::mat4 object2d::get_local_transform() const {
		if (m_isDirty) {
			m_transform = glm::mat4(1.0f); // identity

			// apply transformations
			m_transform = glm::translate(m_transform, glm::vec3(m_position, 0.0f));
			m_transform = glm::rotate(m_transform, m_angle, glm::vec3(0.0f, 0.0f, 1.0f));
			m_transform = glm::scale(m_transform, glm::vec3(m_scale, 1.0f));

			m_isDirty = false;
		}
		return m_transform;
	}

	inline bool object2d::transform_is_dirty() const {
		return m_isDirty;
	}

	inline const glm::vec2& object2d::get_position() const {
		return m_position;
	}

	inline void object2d::set_position(const glm::vec2& position) {
		m_isDirty = true;
		m_position = position;
	}

	inline const glm::vec2& object2d::get_scale() const {
		return m_scale;
	}

	inline void object2d::set_scale(const glm::vec2& scale) {
		m_isDirty = true;
		m_scale = scale;
	}

	inline float object2d::get_rotation() const {
		return m_angle;
	}

	inline void object2d::set_rotation(float rotation) {
		m_isDirty = true;
		m_angle = rotation;
	}

}

#endif // !BE_OBJECT2D_HPP