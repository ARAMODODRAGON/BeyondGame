#ifndef BE_DATATYPES_RANDOM_HPP
#define BE_DATATYPES_RANDOM_HPP
#include <random>
#include <chrono>
#include "../macros.hpp"

namespace be {

	// a random number generator with various functions
	class rng {
	public:
		// generates a random seed
		rng();

		rng(uint64 seed_);

		void seed(uint64 seed_);

		// returns a number between min and max
		template<typename ValueType>
		ValueType range(ValueType min, ValueType max);

		// returns a number between 0 and max
		template<typename ValueType>
		ValueType max(ValueType max);

	private:
		std::mt19937_64 m_generator;
	};

	// global random number generator
	// when the app::run is called the seed is automatically set 
	static inline rng random;


	// impl

	inline rng::rng() {
		seed(std::chrono::steady_clock::now().time_since_epoch().count());
	}

	inline rng::rng(uint64 seed_) {
		seed(seed_);
	}

	inline void rng::seed(uint64 seed_) {
		m_generator.seed(seed_);
	}

	template<typename ValueType>
	inline ValueType rng::range(ValueType min, ValueType max) {
		if constexpr (std::is_floating_point_v<ValueType>) {
			constexpr ValueType scale = ValueType(1) / ValueType(std::mt19937_64::max());
			return ValueType(m_generator()) * scale * (max - min) + min;
		}
		if constexpr (std::is_integral_v<ValueType>) {
			return ValueType(m_generator()) % (max - min) + min;
		}
		return ValueType();
	}

	template<typename ValueType>
	inline ValueType rng::max(ValueType max) {
		return range<ValueType>(ValueType(0), max);
	}

}

#endif // !BE_DATATYPES_RANDOM_HPP