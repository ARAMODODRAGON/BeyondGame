#ifndef BE_GRAPHICS2D_CAMERA2D_HPP
#define BE_GRAPHICS2D_CAMERA2D_HPP
#include "object2d.hpp"
#include "renderer2d.hpp"

namespace be {

	class camera2d : public object2d {
	protected:
		void init() override;
	public:

		camera2d();

		~camera2d();

		void set_bounds(const rect2& bounds);

		void set_size(const glm::vec2& size);

		const glm::mat4& get_view() const;

		bool is_main() const;

		void set_main();

	private:
		glm::vec2 m_viewSize;
		mutable glm::mat4 m_view;
		mutable bool m_isDirty;
		graphics2d::renderer2d* m_renderer2d;
	};

}

#endif // !BE_GRAPHICS2D_CAMERA2D_HPP