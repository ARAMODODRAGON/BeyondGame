#ifndef BE_RECT_HPP
#define BE_RECT_HPP
#include "../common.hpp"

namespace be {

	// a simple representation of a rectangle
	// used for a multitude of applications
	struct rect {
	public:

		union {
			struct {
				float left;
				float bottom;
				float right;
				float top;
			};
			struct {
				glm::vec2 min;
				glm::vec2 max;
			};
		};


		rect();

		rect(float left, float bottom, float right, float top);

		// maps [min] to [left, bottom] and [max] to [right, top]
		rect(const glm::vec2& min, const glm::vec2 max);

		// creates a rect containing both given rects
		rect(const rect& rect0, const rect& rect1);

	};

}

#endif // !BE_RECT_HPP