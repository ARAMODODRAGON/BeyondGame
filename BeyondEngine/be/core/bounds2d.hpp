#ifndef BE_RECT_HPP
#define BE_RECT_HPP
#include "../common.hpp"

namespace be {

	// a simple representation of a 2d boundery
	// used for a multitude of applications
	struct bounds2d {
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


		bounds2d() : min(0.0f, 0.0f), max(0.0f, 0.0f) {}

		bounds2d(float left_, float bottom_, float right_, float top_)
			: left(left_), bottom(bottom_), right(right_), top(top_) {}

		// maps [min] to [left, bottom] and [max] to [right, top]
		bounds2d(const glm::vec2& min_, const glm::vec2 max_)
			: min(min_), max(max_) {}

		// creates a rect containing both given rects
		bounds2d(const bounds2d& rect0, const bounds2d& rect1)
			: left(rect0.left < rect1.left ? rect0.left : rect1.left)
			, bottom(rect0.bottom < rect1.bottom ? rect0.bottom : rect1.bottom)
			, right(rect0.right > rect1.right ? rect0.right : rect1.right)
			, top(rect0.top > rect1.top ? rect0.top : rect1.top) {}

	};

}

#endif // !BE_RECT_HPP