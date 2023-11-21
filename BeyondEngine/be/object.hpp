#ifndef BE_OBJECT_HPP
#define BE_OBJECT_HPP
#include "common.hpp"
#include <concepts>

namespace be {

	// generic class used to represent all objects
	// can be inherited from and can have child objects attached
	class object {
		BE_NO_COPY(object);
		BE_NO_MOVE(object);
	public:

		using iterator = std::vector<object*>::iterator;

		object();
		virtual ~object();

		// gets the parent
		object* get_parent() const;

		// sets the parent
		// returns false if failure to reparent such as if this is the root
		//bool set_parent(object* parent);

		// returns the root object
		object* root();

		// gets the number of children
		size_t size() const;

		// gets the child at n position
		object* get(size_t n);

		// gets the first child by name
		object* get(const std::string& name);

		// gets the child at n position
		const object* get(size_t n) const;

		// returns an iterator pointing to the first child
		iterator begin();

		// returns an iterator pointing to past end of the child array
		iterator end();

		// creates a new child object of type
		template<std::derived_from<object> T = object>
		T* create_child(const std::string& name = "");

		// deletes the object if it is a child of this object
		// warning: do not call this from within the child object itself as the destruction is
		// instantanuious
		bool delete_child(object* child);

		// marks this object to be destroyed
		//void destroy();

		// gets the name
		const std::string& get_name() const;

		// sets the name
		void set_name(const std::string& name);

		// gets the unique identifier
		uint32 get_uid() const;

	private:

		// parents and children
		object* m_parent;
		std::vector<object*> m_children;

		// identifiers
		std::string m_name;
		const uint32 m_uid;

		void _remove_child(object* child);
	};

	/// implementation


	template<std::derived_from<object> T>
	inline T* object::create_child(const std::string& name) {
		// create
		T* obj = new T();
		obj->m_name = name;
		obj->m_parent = this;

		// add
		m_children.push_back(obj);

		// return
		return obj;
	}

}

#endif // !BE_OBJECT_HPP