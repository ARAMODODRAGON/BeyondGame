#ifndef BE_DATATYPES_TAG_HPP
#define BE_DATATYPES_TAG_HPP
#include "..\macros.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace be {

	struct tag;
	class taglist;

	namespace detail {

		static inline uint64 tag_counter = 0;
		static inline std::unordered_map<const char*, uint64> tag_mapping;

		inline uint64 get_tag_id(const char* cstr) {
			auto it = tag_mapping.find(cstr);
			if (it == tag_mapping.end())
				return tag_mapping[cstr] = tag_counter++;
			return it->second;
		}

	}

	struct tag {

		tag() : m_id(-1) {}

		explicit tag(const char* cstr) : m_id(detail::get_tag_id(cstr)) {}

		explicit tag(const std::string& str) : tag(str.c_str()) {}

		bool operator==(const tag& other) const {
			return m_id == other.m_id;
		}

		bool operator!=(const tag& other) const {
			return m_id != other.m_id;
		}

		bool valid() const {
			return m_id != -1;
		}

		uint64 id() const {
			return m_id;
		}

	private:
		uint64 m_id;
	};


}

// custom specialization of std::hash 
template<>
struct std::hash<be::tag> {
	std::size_t operator()(const be::tag& s) const noexcept {
		return std::hash<be::uint64>{}(s.id());
	}
};

namespace be {

	class taglist final {
		using container = std::unordered_set<tag>;
	public:
		using iterator = container::const_iterator;

		taglist() {}

		taglist(const std::initializer_list<tag>& list) {
			for (auto t : list) set(t);
		}

		bool get(tag t) const {
			return m_tags.contains(t);
		}

		void set(tag t) {
			m_tags.emplace(t);
		}

		void unset(tag t) {
			m_tags.erase(t);
		}

		size_t size() const {
			return m_tags.size();
		}

		iterator begin() const {
			return m_tags.begin();
		}

		iterator end() const {
			return m_tags.end();
		}

	private:
		container m_tags;
	};

}

#endif // !BE_DATATYPES_TAG_HPP