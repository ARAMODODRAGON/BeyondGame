#ifndef BE_OBJECT_HPP
#define BE_OBJECT_HPP
#include "../common.hpp"
#include <concepts>

namespace be {

	class object_factory;
	class game_context;

	// generic class used to represent all objects
	// can be inherited from and can have child objects attached
	class object {
		BE_NO_COPY(object);
		BE_NO_MOVE(object);
	protected:

		object();
		virtual ~object();

	public:

		using iterator = std::vector<object*>::iterator;

		// gets the parent
		object* get_parent() const;

		// sets the parent
		// returns false if failure to reparent such as if this is the root
		bool set_parent(object* parent);

		// returns the root object
		object* root() const;

		// gets the number of children
		size_t size() const;

		// gets the child at n position
		object* get(size_t n);

		// gets the child at n position
		const object* get(size_t n) const;

		// returns an iterator pointing to the first child
		iterator begin();

		// returns an iterator pointing to past end of the child array
		iterator end();

		// creates a new child object of type
		template<std::derived_from<object> T = object>
		T* create_child(const std::string& name = "");

		// marks this object to be destroyed
		void destroy();

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

		// factory reference
		object_factory* m_factory;

		// bools
		bool m_isMarkedForDestruction : 1;

		void _remove_child(object* child);
		void _mark_for_destruction();
		friend object_factory;
	};

	// class to handle creation, storage, and deletion of objects
	// creates a root object upon creation
	class object_factory final {
	public:

		object_factory();
		~object_factory();

		// creates a root object. if there already is one then it will be deleted
		template<std::derived_from<object> T>
		object* create_root();

		// returns the root object
		object* root();

		// marks an object to be destroyed. this includes all childeren
		// cannot delete root object
		void destroy(object* obj);

		// destroys all the marked objects
		void handle_destruction();

	private:
		object* m_root;
		std::vector<object*> m_objectsToDestroy;
	};


	/// implementation


	template<std::derived_from<object> T>
	inline T* object::create_child(const std::string& name) {
		// create
		T* obj = new T();
		obj->m_name = name;
		obj->m_parent = this;
		obj->m_factory = m_factory;

		// add
		m_children.push_back(obj);

		// return
		return obj;
	}

	template<std::derived_from<object> T>
	inline object* object_factory::create_root() {
		if (m_root) {
			delete m_root;
		}
		m_root = new T();
		return m_root;
	}

}

#endif // !BE_OBJECT_HPP