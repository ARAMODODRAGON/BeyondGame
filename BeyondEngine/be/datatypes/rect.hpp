#ifndef BE_RECT_HPP
#define BE_RECT_HPP
#include <inttypes.h>
#include <glm\glm.hpp>

namespace be {

	using size_t = std::size_t;

	// decl
	template<size_t dim, typename T, glm::qualifier Q = glm::defaultp> struct rect;

	// types
	using rect2 = rect<2, float>;
	using rect3 = rect<3, float>;
	using rect4 = rect<4, float>;

	using drect2 = rect<2, double>;
	using drect3 = rect<3, double>;
	using drect4 = rect<4, double>;

	using irect2 = rect<2, int>;
	using irect3 = rect<3, int>;
	using irect4 = rect<4, int>;

	// template rect struct
	template<size_t dim, typename T, glm::qualifier Q>
	struct rect {
		using vec_t = glm::vec<dim, T, Q>;

		static constexpr size_t dimensions() { return dim; }

		vec_t min;
		vec_t max;

		constexpr rect() = default;

		template<typename Ty>
		explicit constexpr rect(Ty scalar) : min(T(scalar)), max(T(scalar)) {} 

		constexpr rect(const vec_t& min_, const vec_t& max_)
			: min(min_), max(max_) {}

		explicit constexpr rect(const rect<dim, T>& rect0, const rect<dim, T>& rect1)
			: min(glm::min(rect0.min, rect1.min)), max(glm::max(rect0.max, rect1.max)) {}

		// template copy constructor
		template<size_t dim0, typename Ty>
		constexpr rect(const rect<dim0, Ty>& other) : rect(T(0)) {
			for (size_t i = 0; i < (dim0 < dim ? dim0 : dim); i++) {
				min[i] = T(other.min[i]);
				max[i] = T(other.max[i]);
			}
		}

		vec_t& operator[](size_t index) {
			return (&min)[index];
		}

		const vec_t& operator[] (size_t index) const {
			return (&min)[index];
		}

	};


	template<size_t dim, typename T>
	bool intersects(const rect<dim, T>& a, const rect<dim, T>& b) {
		for (size_t i = 0; i < dim; i++) {
			if (a.min[i] > b.max[i] || a.max[i] < b.min[i])
				return false;
		}
		return true;
	}


	template<size_t dim, typename T>
	bool contains(const rect<dim, T>& container, const rect<dim, T>& smaller) {
		for (size_t i = 0; i < dim; i++) {
			if (container.min[i] > smaller.min[i] || container.max[i] < smaller.max[i])
				return false;
		}
		return true;
	}

	template<size_t dim, typename T>
	rect<dim, T> sum(const rect<dim, T>& a, const rect<dim, T>& b) {
		rect<dim, T> x;
		for (size_t i = 0; i < dim; i++) {
			x.min[i] = glm::min(a.min[i], b.min[i]);
			x.max[i] = glm::max(a.max[i], b.max[i]);
		}
		return x;
	}

	template<size_t dim, typename T>
	rect<dim, T> diff(const rect<dim, T>& a, const rect<dim, T>& b) {
		rect<dim, T> x;
		for (size_t i = 0; i < dim; i++) {
			x.min[i] = glm::max(a.min[i], b.min[i]);
			x.max[i] = glm::min(a.max[i], b.max[i]);
		}
		return x;
	}


	template<size_t dim, typename T>
	T area(const rect<dim, T> x) {
		T total = x.min[0] - x.max[0];
		for (size_t i = 1; i < dim; i++) {
			total *= x.min[i] - x.max[i];
		}
		return total;
	}

}

#endif // !BE_RECT_HPP