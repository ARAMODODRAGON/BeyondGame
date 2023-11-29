#ifndef BOE_COMMON_HPP
#define BOE_COMMON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <list>
#include <inttypes.h>
#include <memory>

#include <glm\glm.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "core\function.hpp"
#include "core\logger.hpp"
#include "core\input.hpp"

namespace be {

	using int8 = int8_t;
	using int16 = int16_t;
	using int32 = int32_t;
	using int64 = int64_t;
	using uint8 = uint8_t;
	using uint16 = uint16_t;
	using uint32 = uint32_t;
	using uint64 = uint64_t;

	using size_t = std::size_t;

	template<typename T> using unique = std::unique_ptr<T>;
	template<typename T> using shared = std::shared_ptr<T>;
	template<typename T> using weak = std::weak_ptr<T>;

}

// color macros
#define BE_COLOR_BLACK glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define BE_COLOR_WHITE glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)

// deletes the copy contructor/operator
#define BE_NO_COPY(Type)				\
Type(const Type&) = delete;				\
Type& operator=(const Type&) = delete;

// deletes the move constructor/operator
#define BE_NO_MOVE(Type)				\
Type(Type&&) = delete;					\
Type& operator=(Type&&) = delete;

// prevents instances of this class from being made
#define BE_STATIC_CLASS(Type)			\
BE_NO_COPY(Type); BE_NO_MOVE(Type);		\
Type() = delete;

#endif // !BOE_COMMON_HPP