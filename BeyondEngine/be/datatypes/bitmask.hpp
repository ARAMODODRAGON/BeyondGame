#ifndef BE_BITMASK_HPP
#define BE_BITMASK_HPP
#include <inttypes.h>

namespace be {

	template<typename T>
	struct bitmask;

	using bitmask8 = bitmask<int8_t>;
	using bitmask16 = bitmask<int16_t>;
	using bitmask32 = bitmask<int32_t>;
	using bitmask64 = bitmask<int64_t>;


	template<typename T>
	struct bitmask {
		struct bitindex;
		static constexpr size_t size() { return sizeof(T); }

		static constexpr T none = T(0);
		static constexpr T all = ~T(0);

		constexpr bitmask() : m_val(T(0)) {}

		constexpr bitmask(T val) : m_val(val) {}

		constexpr T value() const { return m_val; }

		operator bool() const { return (bool)m_val; }

		// returns an indexable bit
		bitindex operator[](size_t index) {
			return bitindex(m_val, index);
		}

		// returns an indexable bit
		const bitindex operator[](size_t index) const {
			return bitindex(m_val, index);
		}

		bitmask operator~() const {
			return bitmask(~m_val);
		}

		bitmask operator&(const bitmask& other) const {
			return bitmask(m_val & other.m_val);
		}

		bitmask operator|(const bitmask& other) const {
			return bitmask(m_val | other.m_val);
		}

		bitmask operator^(const bitmask& other) const {
			return bitmask(m_val ^ other.m_val);
		}

		bitmask operator<<(int shift) const {
			return bitmask(m_val << shift);
		}

		bitmask operator>>(int shift) const {
			return bitmask(m_val >> shift);
		}

		bitmask& operator&=(const bitmask& other) {
			m_val &= other.m_val;
			return *this;
		}

		bitmask& operator|=(const bitmask& other) {
			m_val |= other.m_val;
			return *this;
		}

		bitmask& operator^=(const bitmask& other) {
			m_val ^= other.m_val;
			return *this;
		}

		bitmask& operator<<=(int shift) {
			m_val <<= shift;
			return *this;
		}

		bitmask& operator>>=(int shift) {
			m_val &= shift;
			return *this;
		}

		struct bitindex {
			bitindex(T& val, size_t index) {}

			bitindex& operator=(bool v) {
				m_val |= (T(v) << m_index);
				return *this;
			}

			operator bool() const {
				return m_val & (1 << m_index);
			}

		private:
			T& m_val;
			const size_t m_index;
		};
	private:
		T m_val;
	};


}

#endif // !BE_BITMASK_HPP