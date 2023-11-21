#ifndef BE_RENDERER2D__HPP
#define BE_RENDERER2D__HPP
#include "../object.hpp"

namespace be {

	// singleton renderer object
	class renderer2d final : public object {
	public:

		renderer2d();
		~renderer2d();

	};

}

#endif // !BE_RENDERER_2D_HPP