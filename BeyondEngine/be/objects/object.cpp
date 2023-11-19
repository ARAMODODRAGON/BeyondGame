#include "object.hpp"

namespace be {

	static uint32 uidcounter = 0;

	object::object()
		: m_parent(nullptr)
		, m_uid(uidcounter++)
		, m_factory(nullptr)
		, m_isMarkedForDestruction(false) { }

	object::~object() {
		// assume responsibility
		for (size_t i = 0; i < m_children.size(); i++) {
			delete m_children[i];
		}
		m_children.clear();
	}

	object* object::get_parent() const {
		return m_parent;
	}

	bool object::set_parent(object* parent) {
		// check if null
		if (parent == nullptr) return false;

		// check if this is to be destroyed
		if (m_isMarkedForDestruction) {
			BE_WARNING("Cannot reparent object because it is to be destroyed");
			return false;
		}

		// check if we are the root
		if (root() == this) {
			BE_WARNING("Cannot reparent object \"" + m_name + "\" due to it being the root object");
			return false;
		}

		// check that this is not 'this' or a child of 'this'
		object* o = parent;
		while (o != nullptr) {
			if (o == this) {
				BE_WARNING("Cannot reparent object \"" + m_name + "\" due to the new parent \""
						   + parent->m_name + "\"  being related to itself");
				return false;
			}
			o = o->get_parent();
		}

		// now reparent
		m_parent->_remove_child(this);
		parent->m_children.push_back(this);
		m_parent = parent;
	}

	object* object::root() const {
		return m_factory->root();
	}

	size_t object::size() const {
		return m_children.size();
	}

	object* object::get(size_t n) {
		return m_children[n];
	}

	const object* object::get(size_t n) const {
		return m_children[n];
	}

	be::object::iterator object::begin() {
		return m_children.begin();
	}

	be::object::iterator be::object::end() {
		return m_children.end();
	}

	void object::destroy() {
		if (root() == this) {
			BE_WARNING("Cannot destroy object \"" + m_name + "\" because it is the root");
			return;
		}
		if (!m_isMarkedForDestruction) {
			m_factory->destroy(this);
			_mark_for_destruction();
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

	void object::_remove_child(object* child) {
		for (size_t i = 0; i < m_children.size(); i++) {
			if (m_children[i] == child) {
				m_children.erase(m_children.begin() + i);
				return;
			}
		}
		BE_WARNING("Unable to remove child \"" + child->m_name + "\" from \"" + m_name + "\"");
	}

	void object::_mark_for_destruction() {
		// mark this and all children
		m_isMarkedForDestruction = true;
		for (size_t i = 0; i < m_children.size(); i++) {
			m_children[i]->_mark_for_destruction();
		}
	}


	object_factory::object_factory() : m_root(nullptr) { 
		m_root = new object();
	}

	object_factory::~object_factory() { 
		delete m_root; m_root = nullptr;
	}

	object* object_factory::root() {
		return m_root;
	}

	void object_factory::destroy(object* obj) { 
		// dont destroy if root or nullptr
		if (obj == m_root || obj == nullptr) return;
		// reserve extra memory
		if (m_objectsToDestroy.capacity() == m_objectsToDestroy.size()) 
			m_objectsToDestroy.reserve(m_objectsToDestroy.capacity() * 2);
		// add to list
		m_objectsToDestroy.push_back(obj);
	}

	void object_factory::handle_destruction() { 
		for (size_t i = 0; i < m_objectsToDestroy.size(); i++) {
			delete m_objectsToDestroy[i];
		}
		m_objectsToDestroy.clear();
	}

}