#ifndef BE_SPRITE_BATCH_HPP
#define BE_SPRITE_BATCH_HPP
#include "../common.hpp"
#include "../core/rect.hpp"
#include "../core/content/shader.hpp"
#include "../core/content/texture2d.hpp"

namespace be {

	class sprite_batch {
	public:

		sprite_batch();
		~sprite_batch();

		// starts sprite batch proccess
		void begin(shared<shader> shader, glm::mat3 camera);

		// adds a sprite to the batch
		// will break the batch if it reaches the maximum textures allowed
		void draw(const rect& drawRect,
				  shared<texture2d> texture,
				  const rect& textureUVs = rect(0.0f, 0.0f, 1.0f, 1.0f),
				  const glm::mat3& transform = glm::mat3(1.0f));

		// finishes drawing the batch
		void end();

	private:

		struct vertex {
			glm::vec2 position;
			glm::vec2 uv;
			uint32 textureindex = -1;
		};

		shared<shader> m_shader;
		std::vector<texture2d> m_textures;
		int m_maxTextures;
		std::vector<vertex> m_verticies;

		uint32 m_vao, m_vbo;
	};
}

#endif // !BE_SPRITE_BATCH_HPP