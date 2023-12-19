#ifndef BE_REFLECTION_TYPE_HASH_HPP
#define BE_REFLECTION_TYPE_HASH_HPP
#include <type_traits>

#define BE_TYPEHASH_THIS ::be::typehash<decltype(*this)>()

namespace be {

	typedef uint32_t typehash_t;

	namespace detail {
		inline typehash_t type_hash_counter = 0;
	}

	// a simple type hash implementation
	template<typename T>
	typehash_t typehash() {
		// the following static value is only set once for each 
		// instantiation of this function and has seperate storage for each
		static typehash_t hash = (detail::type_hash_counter)++;
		return hash;
	}

}

#endif // !BE_REFLECTION_TYPE_HASH_HPP