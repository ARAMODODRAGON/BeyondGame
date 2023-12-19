#ifndef BE_GRAPHICS2D_RENDERER2D_HPP
#define BE_GRAPHICS2D_RENDERER2D_HPP
#include "..\graphics2d\sprite_batch.hpp"
#include "../object_registry.hpp"
#include "../content/shader.hpp"
#include "../content/texture2d.hpp"
#include <map>

namespace be {

	// fwd decl
	class spriterenderer;
	class camera2d;

	namespace graphics2d {

		// class that handles all 2d object graphics
		class renderer2d final : public singleton_object {
		public:

			renderer2d();

			~renderer2d();

			// renders all 2d graphics from spriterenderers to text and ui elements
			void draw();

			// returns the main camera in use
			camera2d* get_main_camera();

		private:
			camera2d* m_mainCamera;
			sprite_batch* m_batch;
			std::map<int32, std::vector<spriterenderer*>> m_sprites;
			std::unordered_set<camera2d*> m_cameras;
		public:
			void __add_renderer(int32 index, spriterenderer* renderer);
			void __remove_renderer(int32 index, spriterenderer* renderer);
			void __add_camera2d(camera2d* camera);
			void __remove_camera2d(camera2d* camera);
			void __set_main_camera2d(camera2d* camera);
		};

	}
}

#endif // !BE_GRAPHICS2D_RENDERER2D_HPP