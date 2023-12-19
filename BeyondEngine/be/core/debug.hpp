#ifndef BE_LOGGER_HPP
#define BE_LOGGER_HPP
#include <glm\gtx\string_cast.hpp>
#include <mutex>

namespace be {
	namespace core {

		class debug {
			debug() = delete;
		public:

			static void log(const std::string& msg, const std::string& file, size_t line);
			static void trace(const std::string& msg, const std::string& file, size_t line);
			static void warning(const std::string& msg, const std::string& file, size_t line);
			static void error(const std::string& msg, const std::string& file, size_t line);
			static void fatal_error(const std::string& msg, const std::string& file, size_t line);

		private:

			static inline std::mutex s_lock;
			static inline std::string s_logFilepath = "";
			static const std::string& get_log_path();
		};

	}
}

#define BE_LOG(msg) \
	::be::core::debug::log(msg, __FILE__, __LINE__)
//#define BE_TRACE(msg) \
//	::be::debug::trace(msg, __FILE__, __LINE__)
#define BE_WARNING(msg) \
	::be::core::debug::warning(msg, __FILE__, __LINE__)
#define BE_ERROR(msg) \
	::be::core::debug::error(msg, __FILE__, __LINE__)
#define BE_FATAL_ERROR(msg) \
	::be::core::debug::fatal_error(msg, __FILE__, __LINE__)

#endif // !BE_LOGGER_HPP