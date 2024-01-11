#ifndef BE_MACROS_HPP
#define BE_MACROS_HPP
#include <inttypes.h>

// deletes the copy contructor/operator
#define BE_NO_COPY(type)				\
type(const type&) = delete;				\
type& operator=(const type&) = delete;

// deletes the move constructor/operator
#define BE_NO_MOVE(type)				\
type(type&&) = delete;					\
type& operator=(type&&) = delete;

// prevents instances of this class from being made
#define BE_STATIC_CLASS(type)			\
BE_NO_COPY(type); BE_NO_MOVE(type);		\
type() = delete;

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

}
#endif // !BE_MACROS_HPP