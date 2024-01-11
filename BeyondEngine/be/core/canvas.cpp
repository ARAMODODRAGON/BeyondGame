#include "canvas.hpp"
#include <glew\glew.h>

static constexpr const char* sprbatchShaderSrc[] = {R""(
#type vertex
#version 450 core
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec2 a_uvcoords;
layout (location = 2) in vec4 a_color;
layout (location = 3) in int a_textureIndex;
layout (location = 1) uniform mat4 u_transform;

out vec4 v_color;
out vec2 v_uvcoords;
out flat int v_textureIndex;

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
in flat int v_textureIndex;

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

	canvas::canvas(shared<shader> s)
		: m_maxTextureUnits(0), m_vao(0), m_vbo(0), m_bufferSize(0), m_isDirty(true) {

		// get max texture units
		GLint maxTextureUnits = -1;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
		m_maxTextureUnits = maxTextureUnits;

		// generate shader if one is not provided
		if (s) m_shader = s;
		else {
			std::string source = sprbatchShaderSrc[0] + std::to_string(maxTextureUnits) + sprbatchShaderSrc[1];
			m_shader.reset(shader::load(source, shader::IS_SOURCE));
		}
		//m_transformUniformLocation = glGetUniformLocation(*m_shader, "u_transform");

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
		glVertexAttribIPointer(3, 1, GL_INT, sizeof(batchvert), (void*)offsetof(batchvert, textureIndex));

		// unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	canvas::~canvas() {
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
	}

	void canvas::clear() {
		m_lookuptable.clear();
		m_batches.clear();
	}

	void canvas::draw(const glm::mat4& view) {
		// dont draw if no batches (no batches?)
		if (m_batches.size() == 0) return;

		size_t offset = 0;

		// bind and use program
		glUseProgram(*m_shader);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

		// set uniforms
		glUniformMatrix4fv(1, 1, GL_FALSE, &(view[0].x));

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
				glBufferSubData(GL_ARRAY_BUFFER, offset, verts.size() * sizeof(batchvert), verts.data());
				// move offset
				offset += m_batches[i].verticies.size() * sizeof(batchvert);
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

	void canvas::push(
		const rect2& bounds,
		const glm::vec4& color,
		texture2d* texture,
		const rect2* textureBounds,
		const glm::mat4* transform
	) {
		// verts are in clockwise order
		// [1]---[2]
		//  |   / |
		//  | /   |
		// [0]---[3]
		constexpr size_t vertSize = 4;
		batchvert verts[vertSize];
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
		verts[1].position = glm::vec2(bounds.min.x, bounds.max.y);
		verts[2].position = bounds.max;
		verts[3].position = glm::vec2(bounds.max.x, bounds.min.y);

		// transform based on transformation matrix
		if (transform != nullptr) {
			for (size_t i = 0; i < vertSize; i++) {
				verts[i].position = glm::vec2((*transform) * glm::vec4(verts[i].position, 0.0f, 1.0f));
			}
		}

		// set default uvs
		if (textureBounds == nullptr) {
			verts[0].uv = glm::vec2(0.0f, 1.0f);
			verts[1].uv = glm::vec2(0.0f, 0.0f);
			verts[2].uv = glm::vec2(1.0f, 0.0f);
			verts[3].uv = glm::vec2(1.0f, 1.0f);
		}
		// or set based on parameter
		else {
			verts[0].uv = glm::vec2(textureBounds->min.x, textureBounds->max.y);
			verts[1].uv = textureBounds->min;
			verts[2].uv = glm::vec2(textureBounds->max.x, textureBounds->min.y);
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
			b->verticies.reserve((b->verticies.size() + 6) * 2);

		// now push first triangle
		b->verticies.push_back(verts[0]); // [1]-[2]
		b->verticies.push_back(verts[1]); //  | /
		b->verticies.push_back(verts[2]); // [0]
		// and second triangle
		b->verticies.push_back(verts[0]); //     [2]
		b->verticies.push_back(verts[2]); //    / |
		b->verticies.push_back(verts[3]); // [0]-[3]
	}

	glm::vec2 canvas::push(
		const std::string& text,
		font& f,
		const glm::vec4& color,
		const glm::vec2& position_,
		const glm::vec2& scale,
		textalignment alignment,
		float linespacing
	) {
		if (text.size() == 0 || !f.is_valid()) return position_;

		// verts are in clockwise order
		// [1]---[2]
		//  |   / |
		//  | /   |
		// [0]---[3]
		constexpr size_t vertSize = 4;
		batchvert verts[vertSize];
		// there are 4 parameters to set
		// batchvert::position		-> glm::vec2
		// batchvert::uv			-> glm::vec2
		// batchvert::color			-> glm::vec4
		// batchvert::textureIndex	-> uint32

		// get batch and texture index
		batch* b = get_batch(&f);
		uint32 textureIndex = get_index(b, &f);

		// expand batch
		if ((b->verticies.size() + text.size()) >= b->verticies.capacity())
			b->verticies.reserve((b->verticies.size() + text.size() * 6) * 2);

		// color and texture
		for (size_t i = 0; i < vertSize; i++) {
			verts[i].color = color;
			verts[i].textureIndex = textureIndex;
		}

		glm::vec2 position = position_;
		size_t line = 0;
		const float fsize = (float)f.get_font_size();
		const glm::vec2 uvscaleinverse = glm::vec2(1.0f) / glm::vec2(f.get_size());
		const float spacing = (float(linespacing) * fsize + fsize) * scale.y;

		// loop
		for (size_t i = 0; i < text.size(); i++) {

			// newline
			if (text[i] == '\n' || i == 0) {
				if (text[i] == '\n') line++;
				// align position
				position = position_;
				switch (alignment) {
					case be::textalignment::center:
						position.x -= str::string_width(f, text, line) * 0.5f;
						break;
					case be::textalignment::right:
						position.x -= str::string_width(f, text, line);
						break;
					case be::textalignment::left:
					default: break;
				}
				position.y += spacing * float(line);
				// skip this char if its \n
				if (text[i] == '\n') continue;
			}

			auto& chara = f.get_character(text[i]);

			glm::vec2 size = (chara.bounds.max - chara.bounds.min) * scale;
			glm::vec2 realposition = position + glm::vec2(chara.offset.x, chara.offset.y) * scale;

			// [1]---[2]
			//  |   / |
			//  | /   |
			// [0]---[3]

			verts[0].position = realposition + glm::vec2(0.0f, size.y);
			verts[1].position = realposition;
			verts[2].position = realposition + glm::vec2(size.x, 0.0f);
			verts[3].position = realposition + size;


			verts[0].uv = glm::vec2(chara.bounds.min.x, chara.bounds.max.y) * uvscaleinverse;
			verts[1].uv = chara.bounds.min * uvscaleinverse;
			verts[2].uv = glm::vec2(chara.bounds.max.x, chara.bounds.min.y) * uvscaleinverse;
			verts[3].uv = chara.bounds.max * uvscaleinverse;

			// now push first triangle
			b->verticies.push_back(verts[0]); // [1]-[2]
			b->verticies.push_back(verts[1]); //  | /
			b->verticies.push_back(verts[2]); // [0]
			// and second triangle
			b->verticies.push_back(verts[0]); //     [2]
			b->verticies.push_back(verts[2]); //    / |
			b->verticies.push_back(verts[3]); // [0]-[3]

			// advance position
			position.x += chara.advance * scale.x;
		}

		return position;
	}

	void canvas::push(const std::vector<vertex>& verticies, texture2d* texture) {
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

	canvas::batch* canvas::get_batch(texture2d* texture) {
		// we assume a batch will be updated so the vbo will need to update
		m_isDirty = true;

		// if there are no batches add one at least
		if (m_batches.size() == 0) {
			m_batches.emplace_back();
			m_lookuptable[nullptr] = 0; // this is so null textures used the first batch regardless of texture slots
		}

		// check if no batch contains texture (if nullptr garunteed to skip)
		if (!m_lookuptable.contains(texture)) {
			// check for each batch if any has any texture slots available
			for (size_t i = 0; i < m_batches.size(); i++) {
				if (m_batches[i].textures.size() < m_maxTextureUnits) {
					m_batches[i].textures.push_back(texture); // push texture
					m_lookuptable[texture] = i; // create table entry
					return &(m_batches[m_lookuptable[texture]]);
				}
			}
			// if no slots were available then we create a new batch
			m_batches.emplace_back(); // add
			m_batches.back().textures.push_back(texture); // push texture
			m_lookuptable[texture] = m_batches.size() - 1; // create table entry
		}
		// return the batch
		return &(m_batches[m_lookuptable[texture]]);
	}

	uint32 canvas::get_index(batch* b, texture2d* texture) {
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
