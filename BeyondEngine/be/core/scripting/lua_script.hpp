#ifndef BE_LUA_SCRIPT_HPP
#define BE_LUA_SCRIPT_HPP
#include "../../common.hpp"

typedef struct lua_State lua_State;

namespace be {

	//struct lua_caller final {
	//	lua_caller(const char* func);
	//
	//	lua_caller arg(int val);
	//
	//private:
	//	const char* m_func;
	//};

	// a wrapper around a lua script file which is instanced with its own enviroment
	class lua_script final {
	public:

		// creates an instance of the script with an environment
		lua_script(const char* filepath);

		~lua_script();

		// calls a function on the script
		//template<typename... Args>
		//void call(const char* func, Args... args);

		// calls a function on the script with no arguments and no return value
		// returns false if unsuccessful
		bool call(const char* func);

		// returns the name of the environment global variable
		const char* env() const { return m_env.c_str(); }

		// returns the state
		static lua_State* get_state();

	private:
		std::string m_env;
	};



	namespace detail {

		class lua_context final {
		public:

			lua_context();

			~lua_context();

			lua_State* state;


		};

	}

}

#endif // !BE_LUA_SCRIPT_HPP