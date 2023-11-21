#ifndef BE_OBJECT2D_HPP
#define BE_OBJECT2D_HPP
#include "object.hpp"

namespace be {

	// 2d representation of an object
	// position, rotation, and scale are relative to the direct parent object if it is a object2d
	class object2d : public object {
	public:

		object2d();
		virtual ~object2d();

		// gets the position
		const glm::vec2& get_position() const;

		// sets the position
		void set_position(const glm::vec2 position);

		// gets the local position
		const glm::vec2& get_local_position() const;

		// sets the local position
		void set_local_position(const glm::vec2 position);

		// gets the rotation
		float get_rotation() const;

		// sets the rotation
		void set_rotation(float rotation);

		// gets the local rotation
		float get_local_rotation() const;

		// sets the local rotation
		void set_local_rotation(float rotation);

		// gets the scale
		const glm::vec2 get_scale() const;

		// sets the scale
		void set_scale(const glm::vec2& scale);

		// gets the local scale
		const glm::vec2 get_local_scale() const;

		// sets the local scale
		void set_local_scale(const glm::vec2& scale);

	private:
		glm::vec2 m_position;
		float m_rotation;
		glm::vec2 m_scale;
		mutable object2d* m_parent2d;
		mutable bool m_checkonce : 1;
		object2d* _get_parent2d() const;
	};

}

#endif // !BE_OBJECT2D_HPP