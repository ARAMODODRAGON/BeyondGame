#ifndef BOE_COMMON_HPP
#define BOE_COMMON_HPP

#include <type_traits>
#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

#include "macros.hpp"

#include "datatypes\function.hpp"
#include "core\debug.hpp"
#include "core\input.hpp"
#include "datatypes\type_hash.hpp"
#include "datatypes\rect.hpp"
#include "datatypes\bitmask.hpp"
#include "datatypes\transform.hpp"
#include "datatypes\signal.hpp"
#include "datatypes\random.hpp"

namespace be {

	template<typename T> using unique = std::unique_ptr<T>;
	template<typename T> using shared = std::shared_ptr<T>;
	template<typename T> using weak = std::weak_ptr<T>;

	// aliases
	using cstring = const char*;

	// converts color to vec4
	inline glm::vec4 rgb(uint8 r, uint8 g, uint8 b, uint8 a = 255) {
		constexpr glm::vec4 scalar(1.0f / 255.0f);
		return glm::vec4(r, g, b, a) * scalar;
	}

}

// color macros
#define BE_COLOR_BLACK be::rgb(0, 0, 0)
#define BE_COLOR_WHITE be::rgb(255, 255, 255)
#define BE_COLOR_CLEAR be::rgb(0, 0, 0, 0)

#define BE_COLOR_RED be::rgb(255, 0, 0)
#define BE_COLOR_YELLOW be::rgb(255,255,0)
#define BE_COLOR_BLUE be::rgb(0, 0, 255)

#define BE_COLOR_GREEN be::rgb(0, 255, 0)
#define BE_COLOR_ORANGE be::rgb(255,165,0)
#define BE_COLOR_PURPLE be::rgb(255, 0, 255)


#endif // !BOE_COMMON_HPP