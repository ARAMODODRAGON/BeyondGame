#include "object.hpp"

namespace be {

	static uint32 uidcounter = 0;

	object::object()
		: m_parent(nullptr)
		, m_uid(uidcounter++) { }

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

	//bool object::set_parent(object* parent) {
	//	// check if null
	//	if (parent == nullptr) return false;
	//
	//	// check if this is to be destroyed
	//	if (m_isMarkedForDestruction) {
	//		BE_WARNING("Cannot reparent object because it is to be destroyed");
	//		return false;
	//	}
	//
	//	// check if we are the root
	//	if (root() == this) {
	//		BE_WARNING("Cannot reparent object \"" + m_name + "\" due to it being the root object");
	//		return false;
	//	}
	//
	//	// check that this is not 'this' or a child of 'this'
	//	object* o = parent;
	//	while (o != nullptr) {
	//		if (o == this) {
	//			BE_WARNING("Cannot reparent object \"" + m_name + "\" due to the new parent \""
	//					   + parent->m_name + "\"  being related to itself");
	//			return false;
	//		}
	//		o = o->get_parent();
	//	}
	//
	//	// now reparent
	//	m_parent->_remove_child(this);
	//	parent->m_children.push_back(this);
	//	m_parent = parent;
	//}

	object* object::root() {
		// if there is no parent then this must be the root
		if (m_parent == nullptr)
			return this;
		// else then call root() on the parent
		return m_parent->root();
	}

	size_t object::size() const {
		return m_children.size();
	}

	object* object::get(size_t n) {
		return m_children[n];
	}

	object* object::get(const std::string& name) {
		// find
		for (auto* obj : m_children)
			if (obj->m_name == name)
				return obj;
		// else
		return nullptr;
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

	//void object::destroy() {
	//	if (root() == this) {
	//		BE_WARNING("Cannot destroy object \"" + m_name + "\" because it is the root");
	//		return;
	//	}
	//	if (!m_isMarkedForDestruction) {
	//		m_factory->destroy(this);
	//		_mark_for_destruction();
	//	}
	//}

	bool object::delete_child(object* child) {
		// find the object and delete it
		for (size_t i = 0; i < m_children.size(); i++) {
			if (m_children[i] == child) {
				m_children.erase(m_children.begin() + i); // deletes at position 'i'
				delete child;
				return true;
			}
		}
		// if not found
		BE_ERROR("Could not delete object \"" + child->m_name +
				 "\" as it was not a child of \"" + m_name + "\"");
		return false;
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

}