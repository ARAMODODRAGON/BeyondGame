#include "rect.hpp"

namespace be {

	rect::rect()
		: min(0.0f, 0.0f), max(0.0f, 0.0f) { }

	rect::rect(float left_, float bottom_, float right_, float top_)
		: left(left_)
		, bottom(bottom_)
		, right(right_)
		, top(top_) { }

	rect::rect(const glm::vec2& min_, const glm::vec2 max_)
		: min(min_), max(max_) { }

	rect::rect(const rect& rect0, const rect& rect1)
		: left(rect0.left < rect1.left ? rect0.left : rect1.left)
		, bottom(rect0.bottom < rect1.bottom ? rect0.bottom : rect1.bottom)
		, right(rect0.right > rect1.right ? rect0.right : rect1.right)
		, top(rect0.top > rect1.top ? rect0.top : rect1.top) { }

}