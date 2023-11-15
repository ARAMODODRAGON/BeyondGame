#ifndef ALC_CONTENT_SHADER_HPP
#define ALC_CONTENT_SHADER_HPP
#include "../common.hpp"

namespace be {

	// object representing a shader
	// file uses a single file format where #type *x* indicators are used to identify the types of shaders in a file
	// types include:
	// 'vertex', 'fragment', 'geometry', and 'compute' shader types
	class shader {
		BE_NO_COPY(shader);
		BE_NO_MOVE(shader);
	public:
		using loadset = bool;
		static constexpr loadset IS_FILEPATH = 0;
		static constexpr loadset IS_SOURCE = 1;

		// invalid shader
		shader(std::nullptr_t = nullptr);

		// loads a shader 
		// loadset determines if the str is a filepath or sourcecode
		shader(const std::string& str, loadset set = IS_FILEPATH);

		// deletes the shader
		~shader();

		// checks if this is a valid shader
		bool is_valid() const;

		// checks if this is a valid shader
		operator bool() const;

		// returns the id
		operator uint32() const;

		// returns the id
		uint32 get_id() const;

		// gets the uniform id
		uint32 get_uniform(const std::string& str) const;

	private:
		uint32 m_id;
		void loadfile(const std::string& filepath);
		void loadsrc(const std::string& source);
	};

}

#endif // !ALC_CONTENT_SHADER_HPP