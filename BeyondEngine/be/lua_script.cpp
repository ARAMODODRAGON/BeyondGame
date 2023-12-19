#include "lua_script.hpp"
#include <lua.hpp>
#include "core\debug.hpp"

#define DEBUG "debug"
#define INPUT "input"

namespace be {

	static std::unique_ptr<lua_State, void(*)(lua_State*)> s_state(nullptr, lua_close);
	static uint32 s_envcounter = 0;

	static std::string get_unique_environment() {
		return "_ENV_" + std::to_string(s_envcounter++);
	}

	/// lua_script

#pragma region helper functions

	static int __log(lua_State* state) {
		if (!lua_isstring(state, -1)) return 1;
		core::debug::log(lua_tolstring(state, -1, nullptr), "lua", -1);
		return 0;
	}

	static int __warn(lua_State* state) {
		if (!lua_isstring(state, -1)) return 1;
		core::debug::warning(lua_tolstring(state, -1, nullptr), "lua", -1);
		return 0;
	}

	static int __error(lua_State* state) {
		if (!lua_isstring(state, -1)) return 1;
		core::debug::error(lua_tolstring(state, -1, nullptr), "lua", -1);
		return 0;
	}

	static void pushvalue(lua_State* state, std::nullptr_t) {
		lua_pushnil(state);
	}

	static void pushvalue(lua_State* state, bool val) {
		lua_pushboolean(state, (int)val);
	}

	static void pushvalue(lua_State* state, int val) {
		lua_pushinteger(state, (lua_Integer)val);
	}

	static void pushvalue(lua_State* state, float val) {
		lua_pushnumber(state, (lua_Number)val);
	}

	static void pushvalue(lua_State* state, const char* val) {
		lua_pushstring(state, val);
	}

#pragma endregion

	static void init(lua_State* state) {
		// DEBUG
		lua_newtable(state);

		lua_pushcfunction(state, __log);
		lua_setfield(state, -2, "log");

		lua_pushcfunction(state, __warn);
		lua_setfield(state, -2, "warning");

		lua_pushcfunction(state, __error);
		lua_setfield(state, -2, "error");

		lua_setglobal(state, DEBUG);

	}

	lua_State* lua_env::state() {
		if (s_state.get() == nullptr) {
			s_state.reset(luaL_newstate());
			init(s_state.get());
		}
		return s_state.get();
	}

	lua_env::lua_env(const char* filepath)
		: m_env(get_unique_environment()) {
		// get state
		auto* s = this->state();

		// create unique environment for this instance
		lua_newtable(s);

		// add global references
		lua_getglobal(s, DEBUG);
		lua_setfield(s, -2, DEBUG);

		// add environment to globals
		lua_setglobal(s, m_env.c_str());

		// now load script 
		if (luaL_loadfile(s, filepath) != LUA_OK) {
			BE_ERROR("Could not load lua script at: " + std::string(filepath));
			return;
		}

		// set environment
		lua_getglobal(s, m_env.c_str());
		lua_setupvalue(s, -2, 1);

		// run script
		lua_pcall(s, 0, 0, 0);


	}

	lua_env::~lua_env() {
		auto* s = state();
		// delete our environment
		lua_pushnil(s);
		lua_setglobal(s, m_env.c_str()); // [env] = nil
	}

	bool lua_env::call(const char* func) {
		auto* s = state();
		// get function
		lua_getglobal(s, m_env.c_str());
		lua_getfield(s, -1, func);

		// check if it is a function
		if (::lua_type(s, -1) != LUA_TFUNCTION) {
			BE_WARNING("Could not call function: " + std::string(func));
			return false;
		}

		// call function
		lua_pcall(s, 0, 0, 0);

		// pop environment
		lua_pop(s, 1);

		return true;
	}

	void lua_env::set_field(const char* name, const lua_var& value) {
		auto* s = state();
		// get our env
		lua_getglobal(s, m_env.c_str());

		// push value
		if (value.is<lua_nil>()) lua_pushnil(s);
		else if (value.is<lua_boolean>()) lua_pushboolean(s, value.unsafe_get<lua_boolean>());
		else if (value.is<lua_number>()) lua_pushnumber(s, value.unsafe_get<lua_number>());
		else if (value.is<lua_string>()) lua_pushstring(s, value.unsafe_get<lua_string>());

		// cannot push value
		else {
			// pop environment
			lua_pop(s, 1);
			return;
		}

		// set the value
		lua_setfield(s, -2, name);

		// pop environment
		lua_pop(s, 1);
	}

	lua_var lua_env::get_field(const char* name) {
		lua_var val;
		auto* s = state();
		// get our env
		lua_getglobal(s, m_env.c_str());

		// get the field
		lua_getfield(s, -1, name);

		// fill variant based on field
		switch (::lua_type(s, -1)) {
			case LUA_TNIL:		val = nullptr; break;
			case LUA_TBOOLEAN:	val = lua_toboolean(s, -1); break;
			case LUA_TNUMBER:	val = lua_tonumber(s, -1);  break;
			case LUA_TSTRING:	val = lua_tolstring(s, -1, nullptr); break;
			default: break;
		}

		// pop environment
		lua_pop(s, 1);
		// return variant
		return val;
	}


}
