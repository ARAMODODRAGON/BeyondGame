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
		using object_vector = std::vector<unique<object>>;
	protected:

		object();
		~object();

	public:

		using iterator = object_vector::iterator;

		// gets the parent
		object* get_parent() const;

		// sets the parent
		// returns false if failure to reparent
		bool set_parent(object* newparent);

		// returns true if this is the root object
		bool is_root() const;

		// gets the number of children
		size_t size() const;

		// gets the child at n position
		object* get(size_t n);

		// gets the child at n position
		const object* get(size_t n) const;

		// returns an iterator pointing to the begining of the child array
		iterator begin();

		// returns an iterator pointing to the end of the child array
		iterator end();

		// creates a new child object
		object* create_child(const std::string& name = "");

		// creates a new child object of type
		template<std::derived_from<object> T>
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
		object_vector m_children;

		// identifiers
		std::string m_name;
		uint32 m_uid;

		// factory reference
		object_factory* m_factory;

		// bools
		bool isDestroying : 1;

	};

	// class to handle creation, storage, and deletion of objects
	// creates a root object upon creation
	class object_factory final {
	public:

		object_factory();
		~object_factory();

		// returns the root object
		object* get_root();

		// marks an object to be destroyed
		void destroy(object*);

	private:
		object* m_rootObject;
		void destroy_objects();
		friend game_context;
	};

}

#endif // !BE_OBJECT_HPP