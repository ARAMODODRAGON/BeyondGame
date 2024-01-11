#ifndef BE_CONTENT_FONT_HPP
#define BE_CONTENT_FONT_HPP
#include "texture2d.hpp"

namespace be {

	using font_size = size_t;

	inline constexpr size_t c_font_character_count = UCHAR_MAX;

	enum class textalignment : uint8 {
		left,
		center,
		right
	};

	class font final : public texture2d {
		font(uint32 id, glm::uvec2 size, const textureopts& options, font_size fsize);
	public:
		struct character {
			float advance = 0.0f;
			rect2 bounds = rect2(0.0f);
			glm::vec2 offset = glm::vec2(0.0f);
		};

		// loads a font at the given filepath
		// returns nullptr if it couldnt load the font
		static font* load(const std::string& filepath, font_size fontsize);

		~font(); // public destructor

		// returns this fonts size
		font_size get_font_size();

		// returns the character data
		character& get_character(char c);

		const character& get_character(char c) const;

	private:
		font_size m_size;
		character m_characters[c_font_character_count];
	};

	// some helper functions
	namespace str {

		// returns the total width of the string up until the '\n' character is reached
		// line determines which '\n' seperated line should be sized
		float string_width(const font& f, const std::string& text, size_t line);

		// basic line width function using a range
		float cptr_width(const font& f, const char* ptr, const char* end);

		// returns the number of lines in the string
		size_t line_count(const std::string& text);

		// returns the total height of the text with the given spacing
		float string_height(const font& f, const std::string& text, float spacing);

	}

}

#endif // !BE_CONTENT_FONT_HPP