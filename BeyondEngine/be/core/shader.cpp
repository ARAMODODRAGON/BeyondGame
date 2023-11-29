#include "shader.hpp"
#include <fstream>
#include <glew/glew.h>
#include <unordered_map>

namespace be {

	static uint32 getshadertype(const std::string& shadertype) {
		if (shadertype == "vertex") return GL_VERTEX_SHADER;
		if (shadertype == "fragment") return GL_FRAGMENT_SHADER;
		if (shadertype == "geometry") return  GL_GEOMETRY_SHADER;
		if (shadertype == "compute") return  GL_COMPUTE_SHADER;
		return -1;
	}

	shader::shader(const std::string& str, loadset set) {
		if (set == IS_FILEPATH)
			loadfile(str);
		else if (set == IS_SOURCE)
			loadsrc(str);
	}

	shader::~shader() {
		if (is_valid()) glDeleteProgram(m_id);
	}

	shader::shader(std::nullptr_t) : m_id(0) { }

	bool shader::is_valid() const {
		return m_id != 0;
	}

	shader::operator bool() const { return m_id != 0; }

	shader::operator uint32() const { return m_id; }

	uint32 shader::get_id() const {
		return m_id;
	}

	uint32 shader::get_uniform(const std::string& str) const {
		return glGetUniformLocation(get_id(), str.c_str());
	}

	void shader::loadfile(const std::string& filepath) {

		std::ifstream file(filepath);
		if (!file.is_open()) {
			BE_ERROR("Shader path was invalid: " + filepath);
			return;
		}

		// read file into string
		std::string source;
		file.seekg(0, std::ios::end);
		source.resize(static_cast<size_t>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(source.data(), source.size());
		file.close();

		loadsrc(source);
	}

	void shader::loadsrc(const std::string& source) {
		// via "the cherno" https://youtu.be/8wFEzIYRZXg?t=1221

		std::unordered_map<uint32, std::string> sources;

		// read the file for each shader
		static const std::string typeToken = "#type";
		static const size_t typeTokenLen = typeToken.size();
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos) {
			size_t eol = source.find_first_of("\r\n", pos);
			if (eol == std::string::npos) {
				BE_ERROR("Syntax error");
				return;
			}
			size_t begin = pos + typeTokenLen + 1;
			uint32 shaderType = getshadertype(source.substr(begin, eol - begin));
			if (shaderType == -1) {
				pos = source.find(typeToken, eol);
				continue;
			}
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos + 2);
			size_t endPos = pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos);
			sources.emplace(shaderType, source.substr(nextLinePos, endPos));
		}

		// end 'the cherno'

		if (sources.size() == 0) {
			BE_ERROR("Invalid shader source");
			return;
		}

		std::vector<uint32> shaders;
		uint32 shaderProgram = glCreateProgram();

		for (auto& [type_, source_] : sources) {
			// create and load the shader
			uint32 shaderID = glCreateShader(type_);
			const char* csource = source_.c_str();
			glShaderSource(shaderID, 1, &csource, 0);
			glCompileShader(shaderID);

			// check for errors
			int32 success;
			glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
			if (!success) {
				// get error and print
				char infoLog[512];
				glGetShaderInfoLog(shaderID, 512, 0, infoLog);
				BE_ERROR("Failed to compile shader: " + std::string(infoLog));
				BE_LOG(source_);

				// delete the shader and return
				glDeleteShader(shaderID);
				continue;
			}

			glAttachShader(shaderProgram, shaderID);
			shaders.push_back(shaderID);
		}

		// no shaders were attached
		if (shaders.size() == 0) {
			BE_ERROR("Could not load any shaders, deleted shader program");
			glDeleteProgram(shaderProgram);
			return;
		}

		// link
		glLinkProgram(shaderProgram);

		// check linking errors
		GLint success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			// get error message and print
			GLchar infoLog[512];
			glGetProgramInfoLog(shaderProgram, 512, 0, infoLog);
			BE_ERROR("Failed to link program: \n" + std::string(infoLog));
			// delete shaders/program
			for (GLuint id : shaders) glDeleteShader(id);
			glDeleteProgram(shaderProgram);
			return;
		}

		// delete shaders
		for (GLuint id : shaders) glDeleteShader(id);
		shaders.clear();

		// assign program ID and finish
		m_id = shaderProgram;
	}

}