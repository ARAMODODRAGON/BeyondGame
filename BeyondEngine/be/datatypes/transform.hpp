#ifndef BE_CORE_TRANSFORM_HPP
#define BE_CORE_TRANSFORM_HPP
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

namespace be {

	// decl
	template<size_t Dim, typename T, glm::qualifier Q = glm::defaultp> struct transform;

	using transform2d = transform<2, float, glm::defaultp>;
	using transform3d = transform<3, float, glm::defaultp>;

	// a template specialization for 2d space
	template<typename T, glm::qualifier Q>
	struct transform<2, T, Q> {
		using value_t = T;
		using vec_t = glm::vec<2, value_t, Q>;

		// data
		vec_t position;
		vec_t scale;
		value_t rotation;

		transform() : position(0.0f), scale(1.0f), rotation(0.0f) {}

	};


	// a template specialization for 3d space
	template<typename T, glm::qualifier Q>
	struct transform<3, T, Q> {
		using value_t = T;
		using vec_t = glm::vec<3, value_t, Q>;
		using quat_t = glm::qua<value_t, Q>;

		vec_t position;
		vec_t scale;
		quat_t rotation;

		transform() : position(0.0f), scale(1.0f), rotation(0.0f) {}

	};

	// returns a transformation matrix from the transform2d
	template<typename T, glm::qualifier Q>
	glm::mat<4, 4, T, Q> get_mat4(const transform<2, T, Q>& t) {
		glm::mat<4, 4, T, Q> m(1.0f);
		m = glm::translate(m, glm::vec3(t.position, 0.0f));
		m = glm::rotate(m, t.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		m = glm::scale(m, glm::vec3(t.scale, 1.0f));
		return m;
	}

	// returns a transformation matrix from the transform3d
	template<typename T, glm::qualifier Q>
	glm::mat<4, 4, T, Q> get_mat4(const transform<3, T, Q>& t) {
		glm::mat<4, 4, T, Q> m(1.0f);
		m = glm::translate(m, t.position);
		m = m * glm::toMat4(t.rotation);
		m = glm::scale(m, t.scale);
		return m;
	}
}

#endif // !BE_CORE_TRANSFORM_HPP