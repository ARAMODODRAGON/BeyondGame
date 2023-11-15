#include "object.hpp"

namespace be {

	static uint32 uidcounter = 0;

	object::object() : m_parent(nullptr), m_uid(uidcounter++), m_factory(nullptr), isDestroying(false) { }

	object::~object() { }

	object* object::get_parent() const {
		return m_parent;
	}

	bool object::set_parent(object* newparent) {
		if (is_root()) return false;
		// TODO
	}

	bool object::is_root() const {
		if (m_factory)
			return this == m_factory->get_root();
	}

	size_t object::size() const {
		return m_children.size();
	}

	object* object::get(size_t n) {
		return m_children[n].get();
	}

	const object* object::get(size_t n) const {
		return m_children[n].get();
	}

	be::object::iterator object::begin() {
		return m_children.begin();
	}

	be::object::iterator be::object::end() {
		return m_children.end();
	}

	object* object::create_child(const std::string& name) {
		// TODO
	}

	void object::destroy() {
		if (!m_factory) {


		}
		if (!isDestroying) {
			m_factory->destroy(this);
			isDestroying = true;
		}
	}

	const std::string& object::get_name() const {
		return m_name;
	}

	void object::set_name(const std::string& name) {
		m_name = name;
	}

	uint32 object::get_uid() const {
		return m_uid;
	}

}