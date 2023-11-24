#include "sprite_batch.hpp"
#include <glew\glew.h>

namespace be {

	sprite_batch::sprite_batch()
		: m_shader(nullptr)
		, m_maxTextures(-1)
		, m_vao(-1)
		, m_vbo(-1) {

		// first we check how many available texture units are available for this system
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextures);

		// and allocated our vector to this number
		m_textures.reserve(m_maxTextures);

		// allocate memory for the vertices
		m_verticies.reserve(500);

		// create the VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// create the vertex buffer
		glGenBuffers(1, &m_vbo);
		glBindBuffer(m_vbo, GL_ARRAY_BUFFER);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);

		// determine attributes that are used regardless of shader

	}

	sprite_batch::~sprite_batch() { }

	void sprite_batch::begin(shared<shader> shader, glm::mat3 camera) { }

	void sprite_batch::draw(
		const rect& drawRect,
		shared<texture2d> texture,
		const rect& textureUVs,
		const glm::mat3& transform
	) {


	}

	void sprite_batch::end() { }

}