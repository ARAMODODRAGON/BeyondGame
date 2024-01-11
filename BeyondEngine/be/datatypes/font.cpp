#include "font.hpp"
#include <glew\glew.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "..\dependencies\stb_truetype.h"
#include <fstream>
#include <iostream>

namespace be {

	//static void freeftlibrary(FT_LibraryRec_* ftlib) {
	//	FT_Done_FreeType(ftlib);
	//}
	//
	//static std::unique_ptr<FT_LibraryRec_, void(*)(FT_LibraryRec_*)> s_ftlibrary(nullptr, freeftlibrary);
	//
	//static FT_Library get_lib() {
	//	if (s_ftlibrary.get() == nullptr) {
	//		FT_Library lib;
	//		if (FT_Init_FreeType(&lib)) {
	//			BE_ERROR("Font library did not initialized.");
	//			return nullptr;
	//		}
	//		s_ftlibrary.reset(lib);
	//	}
	//	return s_ftlibrary.get();
	//}

	static constexpr size_t buffer_sz = 1 << 25; // really big number, change if needed
	static unsigned char buffer[buffer_sz];

	static constexpr size_t bit_w = 100000;
	static constexpr size_t bit_h = 500; // treat as max font hight
	static unsigned char bitmap_buff[bit_w * bit_h];

	font::font(uint32 id, glm::uvec2 size, const textureopts& options, font_size fsize)
		: texture2d(id, size, options)
		, m_size(fsize) {}

	font* font::load(const std::string& filepath, font_size fontsize) {
		if (fontsize > bit_h) return nullptr;

		std::ifstream stream;
		stream.open(filepath, std::ios::binary);

		if (!stream.is_open()) return nullptr;

		stream.read((char*)buffer, buffer_sz);
		stream.close();

		constexpr size_t sz = c_font_character_count;
		stbtt_bakedchar c[sz];
		stbtt_BakeFontBitmap(buffer, 0, (float)fontsize, bitmap_buff, bit_w, bit_h, 0, sz, c);

		int actual_width = 0;
		for (size_t i = 0; i < sz; i++) {
			if (c[i].x1 > actual_width) actual_width = c[i].x1;
		}

		//for (size_t i = 0; i < (bit_w * bit_h); i++) {
		//	bitmap_buff[i] = 255;
		//}

		uint32 textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		glPixelStorei(GL_UNPACK_ROW_LENGTH, bit_w);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, actual_width, fontsize, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap_buff);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);

		textureopts opts;
		opts.filter = texturefilter::linear;
		opts.wrap = texturewrap::clamp_edge;
		opts.format = textureformat::r;

		font* f = new font(textureID, {actual_width, fontsize}, opts, fontsize);

		for (size_t i = 0; i < sz; i++) {
			f->m_characters[i].advance = c[i].xadvance;
			f->m_characters[i].bounds.min.x = c[i].x0;
			f->m_characters[i].bounds.min.y = c[i].y0;
			f->m_characters[i].bounds.max.x = c[i].x1;
			f->m_characters[i].bounds.max.y = c[i].y1;
			f->m_characters[i].offset.x = c[i].xoff;
			f->m_characters[i].offset.y = c[i].yoff;
		}

		return f;
	}

	font::~font() {}

	font_size font::get_font_size() {
		return m_size;
	}

	font::character& font::get_character(char c) {
		return m_characters[int(c)]; // offset
	}

	const font::character& font::get_character(char c) const {
		return m_characters[int(c)]; // offset
	}

	namespace str {

		float string_width(const font& f, const std::string& text, size_t line) {
			size_t off = 0;
			while (line > 0) {
				off = text.find_first_of("\n\0", off);
				line--;
			}

			off++;

			size_t endpos = text.find_first_of("\n\0", off);

			return cptr_width(f, text.c_str() + off, text.c_str() + endpos);
		}

		float cptr_width(const font& f, const char* ptr, const char* end) {
			float width = 0.0f;
			for (const char* p = ptr; p != end; p++) {
				width += f.get_character(*p).advance;
			}
			return width;
		}

		size_t line_count(const std::string& text) {
			if (text.size() == 0) return 0;
			size_t count = 1;
			size_t off = 0;
			while (text.find_first_of("\n", off) != std::string::npos) {
				count++;
			}
			return count;
		}

		float string_height(const font& f, const std::string& text, float spacing) {
			size_t linecount = line_count(text);
			float spacetotal = spacing * (linecount - 1);
			float linetotal = (float)((f.get_size().y - 2) * linecount); // NOTE: maybe we need different height for each line?
			return linetotal + spacetotal;
		}


	}
}
