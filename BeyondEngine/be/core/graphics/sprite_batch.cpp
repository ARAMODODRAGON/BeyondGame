#include "sprite_batch.hpp"
#include <glew\glew.h>

static constexpr const char* sprbatchShaderSrc[] = {R""(
#type vertex
#version 450 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uvcoords;
layout (location = 2) in vec4 a_color;
layout (location = 3) in uint a_textureIndex;
layout (location = 1) uniform mat4 u_transform;

out vec4 v_color;
out vec2 v_uvcoords;
out flat uint v_textureIndex;

void main() {

	v_color = a_color;
	v_textureIndex = a_textureIndex;
	v_uvcoords = a_uvcoords;
	vec4 vertex = u_transform * vec4(a_position, 0.0, 1.0);
	gl_Position = vertex;
	
}

#type fragment
#version 450 core
out vec4 out_fragcolor;

const int c_TextureCount = )"", R""(;
layout (binding = 0) uniform sampler2D u_textures[c_TextureCount];

in vec4 v_color;
in vec2 v_uvcoords;
in flat uint v_textureIndex;

void main() {

	// no texture
	if (v_textureIndex == -1) {
		out_fragcolor = v_color;
	} 
	// font texture
	else if (v_textureIndex < -1) {
		out_fragcolor = texture(u_textures[v_textureIndex + c_TextureCount], v_uvcoords).r * v_color;
	} 
	// normal texture
	else {
		out_fragcolor = texture(u_textures[v_textureIndex], v_uvcoords) * v_color;
	} 
	
}

)""};

namespace be {

	sprite_batch::sprite_batch(shared<shader> s)
		: m_maxTextureUnits(0), m_vao(0), m_vbo(0), m_bufferSize(0), m_isDirty(true) {

		// get max texture units
		GLint maxTextureUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		m_maxTextureUnits = maxTextureUnits;

		// generate shader if one is not provided
		if (s) m_shader = s;
		else {
			std::string source = sprbatchShaderSrc[0] + std::to_string(maxTextureUnits) + sprbatchShaderSrc[1];
			m_shader.reset(new shader(source, shader::IS_SOURCE));
		}

		// generate vertex array
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// generate buffer
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW);

		// assign attributes

		// 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(batchvert), (void*)offsetof(batchvert, position));

		// 1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(batchvert), (void*)offsetof(batchvert, uv));

		// 2
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(batchvert), (void*)offsetof(batchvert, color));

		// 3
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(batchvert), (void*)offsetof(batchvert, textureIndex));

		// unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	sprite_batch::~sprite_batch() {
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}

	void sprite_batch::clear() {
		m_lookuptable.clear();
		m_batches.clear();
	}

	void sprite_batch::draw(const glm::mat4& view) {
		// dont draw if no batches
		if (m_batches.size() == 0) return;

		size_t offset = 0;

		// bind and use program
		glUseProgram(*m_shader);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		// set uniforms
		glUniform1fv(1, sizeof(glm::mat4), &(view[0][0]));

		// if we need to update vbo data
		if (m_isDirty) {

			// get total size
			size_t totalBytes = 0;
			for (size_t i = 0; i < m_batches.size(); i++)
				totalBytes += m_batches[i].verticies.size();
			totalBytes *= sizeof(batchvert);

			// again we're not going to draw if totalBytes is 0
			if (totalBytes == 0) return; // this should never evaluate to true

			// resize the vbo to fit
			if (m_bufferSize < totalBytes) {
				// get a fitting size for the buffer
				m_bufferSize = glm::max<size_t>(500 * sizeof(batchvert), totalBytes * 2);
				// set size
				glBufferData(GL_ARRAY_BUFFER, m_bufferSize, nullptr, GL_STREAM_DRAW);
			}

			// now fill the vbo with data
			for (size_t i = 0; i < m_batches.size(); i++) {
				auto& [textures, verts] = m_batches[i];
				// fill part of buffer
				glBufferSubData(GL_ARRAY_BUFFER, offset, verts.size(), verts.data());
				// move offset
				offset += m_batches[i].verticies.size();
			}

			// set this to make sure we dont regenerate the batch if we dont need to
			m_isDirty = false;
		}

		// now draw each portion of the buffer
		offset = 0;
		for (size_t i = 0; i < m_batches.size(); i++) {
			auto& [textures, verts] = m_batches[i];

			// load in textures
			for (size_t i = 0; i < textures.size(); i++) {
				glActiveTexture(GL_TEXTURE0 + i); // set texture slot
				glBindTexture(GL_TEXTURE_2D, textures[i]->get_id()); // bind texture to slot
			}

			// draw section of buffer
			glDrawArrays(GL_TRIANGLES, offset, verts.size());

			// move offset
			offset += verts.size();
		}
	}

	void sprite_batch::push(const bounds2d bounds, texture2d* texture, const glm::vec4& color, const bounds2d* textureBounds) {
		// verts are in clockwise order
		// [1]---[2]
		//  |   / |
		//  | /   |
		// [0]---[3]
		constexpr size_t vertSize = 4;
		static batchvert verts[vertSize];
		// there are 4 parameters to set
		// batchvert::position		-> glm::vec2
		// batchvert::uv			-> glm::vec2
		// batchvert::color			-> glm::vec4
		// batchvert::textureIndex	-> uint32

		// get batch and texture index
		batch* b = get_batch(texture);
		uint32 textureIndex = get_index(b, texture);

		// set vert positions
		verts[0].position = bounds.min;
		verts[1].position = glm::vec2(bounds.left, bounds.top);
		verts[2].position = bounds.max;
		verts[3].position = glm::vec2(bounds.right, bounds.bottom);

		// set default uvs
		if (textureBounds == nullptr) {
			verts[0].uv = glm::vec2(0.0f, 1.0f);
			verts[1].uv = glm::vec2(0.0f, 0.0f);
			verts[2].uv = glm::vec2(1.0f, 0.0f);
			verts[3].uv = glm::vec2(1.0f, 1.0f);
		}
		// or set based on parameter
		else {
			verts[0].uv = glm::vec2(textureBounds->left, textureBounds->top);
			verts[1].uv = textureBounds->min;
			verts[2].uv = glm::vec2(textureBounds->right, textureBounds->bottom);
			verts[3].uv = textureBounds->max;
		}

		// set colors and texture index
		for (size_t i = 0; i < vertSize; i++) {
			verts[i].color = color;
			verts[i].textureIndex = textureIndex;
		}

		// now we must push all the verticies onto the batch

		// but first lets expand the batch to fit four more verticies
		if ((b->verticies.size() + 4) >= b->verticies.capacity())
			b->verticies.reserve((b->verticies.size() + 4) * 2);

		// now push first triangle
		b->verticies.push_back(verts[0]); // [1]-[2]
		b->verticies.push_back(verts[1]); //  | /
		b->verticies.push_back(verts[2]); // [0]
		// and second triangle
		b->verticies.push_back(verts[0]); //     [2]
		b->verticies.push_back(verts[2]); //    / |
		b->verticies.push_back(verts[3]); // [0]-[3]
	}

	void sprite_batch::push(const std::vector<sprite_vertex>& verticies, texture2d* texture) {
		static batchvert vert;

		// skip this if there are no verticies or if it is not a multiple of 3
		if (verticies.size() == 0 || (verticies.size() % 3) != 0) return;

		// get batch and texture index
		batch* b = get_batch(texture);
		uint32 textureIndex = get_index(b, texture);

		// lets expand the batch to fit all our verticies
		if ((b->verticies.size() + verticies.size()) >= b->verticies.capacity())
			b->verticies.reserve((b->verticies.size() + verticies.size()) * 2);

		// now we're gonna submit all the verticies
		for (size_t i = 0; i < verticies.size(); i++) {
			vert.position = verticies[i].position;
			vert.uv = verticies[i].uv;
			vert.color = verticies[i].color;
			vert.textureIndex = textureIndex;
			// push
			b->verticies.push_back(vert);
		}
	}

	sprite_batch::batch* sprite_batch::get_batch(texture2d* texture) {
		// we assume a batch will be updated so the vbo will need to update
		m_isDirty = true;
		// if no matching batch was found then create new and add to lookup
		if (!m_lookuptable.contains(*texture)) {
			m_batches.emplace_back();
			m_lookuptable[texture->get_id()] = (m_batches.size() - 1);
		}
		// return the batch
		return &(m_batches[m_lookuptable[*texture]]);
	}

	uint32 sprite_batch::get_index(batch* b, texture2d* texture) {
		// return -1 for a non texture
		if (texture == nullptr) return -1;
		// return the index
		for (size_t i = 0; i < b->textures.size(); i++) {
			if (b->textures[i] == texture) {
				return i;
			}
		}
		// we should never get here under normal circumstances
		return -1;
	}

}