#ifndef BE_GRAPHICS2D_SPRITE_BATCH_HPP
#define BE_GRAPHICS2D_SPRITE_BATCH_HPP
#include "..\common.hpp"
#include "..\core\rect.hpp"
#include "..\content\shader.hpp"
#include "..\content\texture2d.hpp"

namespace be {

	namespace graphics2d {

		struct sprite_vertex {
			glm::vec2 position;
			glm::vec2 uv;
			glm::vec4 color;
		};

		// a sprite batching system
		// batches are grouped based on the available texture units of the system
		class sprite_batch {
		public:

			// constructs a sprite batch with an optional shader
			// if no shader is provided then a basic sprite shader is used
			sprite_batch(shared<content::shader> s = nullptr);

			~sprite_batch();

			// clears the memory in the batch
			void clear();

			// draws all sprites currently in the batch
			void draw(const glm::mat4& view);

			// pushes a sprite into the batch
			void push(
				const rect2& bounds,
				const glm::vec4& color,
				content::texture2d* texture = nullptr,
				const rect2* textureBounds = nullptr,
				const glm::mat4* transform = nullptr
			);

			// pushes a set of verticies onto the batch with an accociated texture2d
			// verticies are drawn as a series of triangles so size() must be a multiple of 3
			void push(const std::vector<sprite_vertex>& verticies, content::texture2d* texture = nullptr);

		private:

			struct batchvert {
				glm::vec2 position;
				glm::vec2 uv;
				glm::vec4 color;
				uint32 textureIndex;
			};

			struct batch {
				std::vector<content::texture2d*> textures;
				std::vector<batchvert> verticies;
			};

			shared<content::shader> m_shader;
			std::vector<batch> m_batches;
			std::unordered_map<content::texture2d*, uint32> m_lookuptable;

			uint32 m_maxTextureUnits;
			uint32 m_vao, m_vbo;
			uint32 m_bufferSize;
			bool m_isDirty;

			batch* get_batch(content::texture2d* texture);
			uint32 get_index(batch* b, content::texture2d* texture);
		};
	}
}

#endif // !BE_GRAPHICS2D_SPRITE_BATCH_HPP