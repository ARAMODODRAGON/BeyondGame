#ifndef BE_LUA_SCRIPT_HPP
#define BE_LUA_SCRIPT_HPP
#include "common.hpp"
#include <lua.hpp>

namespace be {

	// lua value types
	using lua_nil = std::nullptr_t;
	using lua_boolean = int;
	using lua_number = double;
	using lua_string = const char*;
	using lua_userdata = void*;
	struct lua_thread {};
	struct lua_table {};
	struct lua_function {};

	// concept used to determine if the given type is a lua type
	template<typename T>
	concept typeof_lua =
		std::is_same_v<T, lua_nil> ||
		std::is_same_v<T, lua_boolean> ||
		std::is_same_v<T, lua_number> ||
		std::is_same_v<T, lua_string> ||
		std::is_same_v<T, lua_userdata> ||
		std::is_same_v<T, lua_thread> ||
		std::is_same_v<T, lua_table> ||
		std::is_same_v<T, lua_function>;

	// represents a lua value as a type safe union akin to std::variant with simplified accessors
	// when not initialized, default to nil
	// will not store the following types: lua_thread, lua_table, lua_function
	// instead only allowing you to check for the type 
	// used only to store simple values
	class lua_var final {
	public:
		enum Type : uint8 {
			nil,
			boolean,
			number,
			string,
			userdata,
			function,
			thread,
			table
		};

		// default initializes to nil
		lua_var(lua_nil = nullptr);

		// sets the value as the given type
		template<typeof_lua T>
		lua_var(T value);

		// returns the type as an integer value
		uint8 type() const;

		// returns if the stored type is whats passed
		template<typeof_lua T> bool is() const;

		// returns the value as the type regardless of what type it should be
		template<typeof_lua T> T&& unsafe_get();

		// returns the value as the type regardless of what type it should be
		template<typeof_lua T> const T&& unsafe_get() const;

		// calls the lambda as the given type
		template<typeof_lua T, typename Lambda>
		void visit(Lambda lambda);

		// calls the lambda as the given type
		template<typeof_lua T, typename Lambda>
		void visit(Lambda lambda) const;

		// calls the lambda on the value regardless of type
		template<typename Lambda>
		void visit_generic(Lambda lambda);

		// calls the lambda on the value regardless of type
		template<typename Lambda>
		void visit_generic(Lambda lambda) const;

	private:
		char m_data[8];
		uint8 m_type;
	};

	// the integer value representing the given type
	template<typeof_lua>
	constexpr uint8 lua_type_value = 0;

	// a wrapper around a lua script file which is instanced with its own enviroment
	class lua_env final {
	public:

		// creates an environment and runs a script within it
		lua_env(const char* filepath);

		~lua_env();

		// calls a function in the environment with no arguments and no return value
		// returns false if unsuccessful
		bool call(const char* func);

		// sets a field within the scripts environment
		void set_field(const char* name, const lua_var& value);

		// gets a field from the scripts environment
		lua_var get_field(const char* name);

		// returns the name of the environment global variable
		const char* env() const { return m_env.c_str(); }

		// returns the state
		static lua_State* state();

	private:
		std::string m_env;
	};




	///////// impl

	inline lua_var::lua_var(lua_nil) : m_data{0}, m_type(nil) {}

	template<typeof_lua T>
	inline lua_var::lua_var(T value) : m_data{0}, m_type(lua_type_value<T>) {
		(*reinterpret_cast<T*>(m_data)) = value;
	}

	inline uint8 lua_var::type() const {
		return m_type;
	}

	template<typeof_lua T>
	inline bool lua_var::is() const {
		return m_type == lua_type_value<T>;
	}

	template<typeof_lua T>
	inline T&& lua_var::unsafe_get() {
		return std::move(*reinterpret_cast<T*>(m_data));
	}

	template<typeof_lua T>
	inline const T&& be::lua_var::unsafe_get() const {
		return std::move(*reinterpret_cast<const T*>(m_data));
	}

	template<typeof_lua T, typename Lambda>
	inline void lua_var::visit(Lambda lambda) {
		if (is<T>()) lambda(unsafe_get<T>());
	}

	template<typeof_lua T, typename Lambda>
	inline void lua_var::visit(Lambda lambda) const {
		if (is<T>()) lambda(unsafe_get<T>());
	}

	template<typename Lambda>
	inline void lua_var::visit_generic(Lambda lambda) {
		if (is<lua_boolean>())	lambda(unsafe_get<lua_boolean>);
		if (is<lua_number>())	lambda(unsafe_get<lua_number>);
		if (is<lua_string>())	lambda(unsafe_get<lua_string>);
		if (is<lua_userdata>())	lambda(unsafe_get<lua_userdata>);
	}

	template<typename Lambda>
	inline void lua_var::visit_generic(Lambda lambda) const {
		if (is<lua_boolean>())	lambda(unsafe_get<lua_boolean>);
		if (is<lua_number>())	lambda(unsafe_get<lua_number>);
		if (is<lua_string>())	lambda(unsafe_get<lua_string>);
		if (is<lua_userdata>())	lambda(unsafe_get<lua_userdata>);
	}

	template<> constexpr uint8 lua_type_value<lua_nil> = lua_var::nil;
	template<> constexpr uint8 lua_type_value<lua_boolean> = lua_var::boolean;
	template<> constexpr uint8 lua_type_value<lua_number> = lua_var::number;
	template<> constexpr uint8 lua_type_value<lua_string> = lua_var::string;
	template<> constexpr uint8 lua_type_value<lua_userdata> = lua_var::userdata;
	template<> constexpr uint8 lua_type_value<lua_thread> = lua_var::thread;
	template<> constexpr uint8 lua_type_value<lua_table> = lua_var::table;
	template<> constexpr uint8 lua_type_value<lua_function> = lua_var::function;

}

#endif // !BE_LUA_SCRIPT_HPP