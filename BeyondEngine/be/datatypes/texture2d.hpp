#ifndef BE_CONTENT_TEXTURE2D_HPP
#define BE_CONTENT_TEXTURE2D_HPP
#include "../common.hpp"

namespace be {

	enum class textureformat : uint8 {
		r, rg, rgb, rgba,

		// used to load the default format of a texture 
		any = static_cast<uint8>(-1)
	};

	enum class texturewrap : uint8 {
		clamp_edge,
		clamp_border,
		repeat,
		mirrored_repeat,
		mirror_clamp_edge
	};

	enum class texturefilter : uint8 {
		linear, nearest
	};

	struct textureopts final {
		textureformat format = textureformat::any;
		texturewrap wrap = texturewrap::clamp_edge;
		texturefilter filter = texturefilter::linear;
	};

	class texture2d {
		BE_NO_COPY(texture2d);
		texture2d();
	public:

		// creates a texture which manages a pre created gl texture
		texture2d(uint32 id, glm::uvec2 size, const textureopts& options = textureopts());

		// loads a texture 
		static texture2d* load(const std::string& filepath, const textureopts& options = textureopts());


		texture2d(texture2d&& other);
		texture2d& operator=(texture2d&& other);

		// deletes a texture
		~texture2d();

		// checks if this is a valid texture
		bool is_valid() const;

		// checks if this is a valid texture
		operator bool() const;

		// check if this texture is the same as the other
		bool operator==(const texture2d& other) const;

		// check if this texture is not the same as the other
		bool operator!=(const texture2d& other) const;

		// returns the texture ID
		uint32 get_id() const;

		// returns the texture size
		glm::uvec2 get_size() const;

		// returns the format of the texture
		textureformat get_format() const;

		// returns the wrap of the texture
		texturewrap get_wrap() const;

		// returns the filter of the texture
		texturefilter get_filter() const;

	private:
		uint32 m_id;
		glm::uvec2 m_size;
		textureformat m_format;
		texturewrap m_wrap;
		texturefilter m_filter;
	};

}

#endif // !BE_CONTENT_TEXTURE2D_HPP