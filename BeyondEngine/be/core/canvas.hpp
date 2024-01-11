#ifndef BE_GRAPHICS2DCANVAS_HPP
#define BE_GRAPHICS2DCANVAS_HPP
#include "..\common.hpp"
#include "..\datatypes\rect.hpp"
#include "..\datatypes\shader.hpp"
#include "..\datatypes\texture2d.hpp"
#include "..\datatypes\font.hpp"

namespace be {

	// a sprite batching system
	// batches are grouped based on the available texture units of the system
	class canvas {
	public:
		struct vertex {
			glm::vec2 position;
			glm::vec2 uv;
			glm::vec4 color;
		};

		// constructs a canvas with an optional shader
		// if no shader is provided then a basic shader is used
		canvas(shared<shader> s = nullptr);

		~canvas();

		// clears the memory in the batch
		void clear();

		// draws all sprites currently in the batch
		void draw(const glm::mat4& view);

		// pushes a sprite into the batch
		void push(
			const rect2& bounds,
			const glm::vec4& color,
			texture2d* texture = nullptr,
			const rect2* textureBounds = nullptr,
			const glm::mat4* transform = nullptr
		);

		// pushes a string with a font into the batch
		// position is the bottom left corner of where the text string will be drawn from
		// returns the last position that it drew from (so you can start a new line of text from there)
		glm::vec2 push(
			const std::string& text,
			font& font_,
			const glm::vec4& color,
			const glm::vec2& position,
			const glm::vec2& scale,
			textalignment alignment,
			float linespacing
		);

		// pushes a set of verticies onto the batch with an accociated texture2d
		// verticies are drawn as a series of triangles so size() must be a multiple of 3
		void push(const std::vector<vertex>& verticies, texture2d* texture = nullptr);

	private:

		struct batchvert {
			glm::vec2 position;
			glm::vec2 uv;
			glm::vec4 color;
			uint32 textureIndex;
		};

		struct batch {
			std::vector<texture2d*> textures;
			std::vector<batchvert> verticies;
		};

		shared<shader> m_shader;
		std::vector<batch> m_batches;
		std::unordered_map<texture2d*, uint32> m_lookuptable;

		uint32 m_maxTextureUnits;
		uint32 m_vao, m_vbo;
		uint32 m_bufferSize;
		bool m_isDirty;

		batch* get_batch(texture2d* texture);
		uint32 get_index(batch* b, texture2d* texture);
	};

}

#endif // !BE_GRAPHICS2DCANVAS_HPP