#include "lua_script.hpp"
#include <lua.hpp>

#define INPUT "_INPUT"
#define DEBUG "_DEBUG"

namespace be {

	static unique<detail::lua_context> s_context;
	static uint32 s_envcounter = 0;

	static std::string get_unique_environment() {
		return "_ENV_" + std::to_string(s_envcounter++);
	}

	detail::lua_context::lua_context() : state(nullptr) {
		// create state
		state = luaL_newstate();
		luaL_openlibs(state);

		// initialize common globals
		// TODO
	}

	/// lua_script
	
	lua_State* lua_script::get_state() {
		if (s_context.get() == nullptr)
			s_context.reset(new detail::lua_context());
		return s_context->state;
	}

	lua_script::lua_script(const char* filepath)
		: m_env(get_unique_environment()) {
		// get state
		auto* state = get_state();

		// create unique environment for this instance
		lua_newtable(state);

		// remove this later
		lua_getglobal(state, "print");
		lua_setfield(state, -2, "print");

		// add environment to globals
		lua_setglobal(state, m_env.c_str());

		// now load script 
		if (luaL_loadfile(state, filepath) != LUA_OK) {
			BE_ERROR("Could not load lua script at: " + std::string(filepath));
			return;
		}

		// set environment
		lua_getglobal(state, m_env.c_str());
		lua_setupvalue(state, -2, 1);

		// run script
		lua_pcall(state, 0, 0, 0);

	}

	lua_script::~lua_script() {
		auto* state = get_state();
		// delete our environment
		lua_pushnil(state);
		lua_setglobal(state, m_env.c_str()); // [env] = nil
	}

	bool lua_script::call(const char* func) {
		auto* state = get_state();
		// get function
		lua_getglobal(state, m_env.c_str());
		lua_getfield(state, -1, func);

		// check if it is a function
		if (!lua_isfunction(state, -1)) {
			BE_WARNING("Could not call function: " + std::string(func));
			return false;
		}

		// call function
		lua_pcall(state, 0, 0, 0);

		return true;
	}

	/// !lua_script

	detail::lua_context::~lua_context() {
		// delete
		lua_close(state);
		state = nullptr;
	}

}
