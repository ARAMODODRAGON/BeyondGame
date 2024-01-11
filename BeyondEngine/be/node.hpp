#ifndef BE_OBJECT_registry_HPP
#define BE_OBJECT_registry_HPP
#include "common.hpp"
#include <unordered_set>
#include "datatypes\type_hash.hpp"

// the type of integer used for hashing types
// -1 is always reserved as a null type
#define BE_OBJECT_INTEGER uint64

namespace be {

	// types defined in this file
	class node;
	class node_tree;
	class singleton_node; // inherits node

	// concept used to constrain all node template functions 
	// excludes singleton_node
	template<typename T>
	concept typeof_node = std::is_base_of_v<node, T> && !std::is_base_of_v<singleton_node, T>;

	// seperate concept for singleton objects so we dont instance singletons when we shouldnt
	// we also cant instance singleton_node itself
	template<typename T>
	concept typeof_singleton_node = std::is_base_of_v<singleton_node, T> && !std::is_same_v<singleton_node, T>;

	// concept used to constrain all node template functions but doesnt exclude singleton_node
	template<typename T>
	concept typeof_anynode = std::is_base_of_v<node, T>;

}

// used to override the factory methods
#define be_factory_methods __befactorymethods__

namespace be_factory_methods {

	// create factory method for creating objects
	// can be overridden
	template<be::typeof_anynode T>
	T* create_method() { return new T(); }

}

namespace be {

	// holds and manages all objects
	class node_tree final {
		BE_NO_COPY(node_tree);
		BE_NO_MOVE(node_tree);
	public:

		// handles deleting objects
		// should be called once per frame
		void handle_deletion();

		// default constructs an node_tree
		node_tree();

		~node_tree();


		// factory method used to construct any node
		template<typeof_node T>
		T* create(node* parent = nullptr, const std::string& name = "");

		// mark an node to be destroyed
		// returns true on success
		// will not destroy singletons
		bool destroy(node* obj);

		// returns a singleton of type
		// if it doesnt exist then a new instance will be created
		template<typeof_singleton_node T> T* get_instance();


		// returns the number of children
		size_t size() const;

		// returns the child node at the given index
		node* at(size_t index);

	private:
		// helper functions
		void __delete(node* obj);
		void __remove_marked(node* obj);
		void __add_root(node* o);
		void __remove_root(node* o);

		// node storage
		std::vector<node*> m_objects;
		std::vector<node*> m_markedForDeletion;
		std::unordered_map<typehash_t, singleton_node*> m_singletons;

		// uid counter
		uint64 m_uidCounter;

		friend node;
	};

	// the based node class
	// can be used as is or inherited from to further develop functionality
	class node {
	protected:

		// initialization function called after node is constructed and added to theregistry
		virtual void init() {}

	public:

		// default constructs node
		// required if not using
		node();

		virtual ~node();


		// returns a reference to the tree that owns this node
		node_tree* tree() const;

		// returns a reference to the parent node 
		// if there is no parent then it will return nullptr
		node* get_parent() const;

		// returns first ancestor node of type 
		// includes direct parent
		// slow call using dynamic_cast<> at each level
		template<typeof_node T>
		T* get_ancestor() const;
		
		// gets the name of this node
		const std::string& get_name() const;

		// sets the name of this node
		void set_name(const std::string& name);

		// gets this objects unique identifier
		uint64 uid() const;

		// gets the activeness of this node
		bool get_active() const;

		// sets the activeness of this node
		void set_active(bool active);


		// creates and attaches an node of type to this node
		template<typeof_node T>
		T* create(const std::string& name = "");

		// creates an node of type and sets the parent
		// if parent is nullptr then it will be created as a root node
		template<typeof_node T>
		T* create(node* parent, const std::string& name = "");

		// mark an node to be destroyed
		// returns true on success
		bool destroy(node* obj);

		// returns a singleton of type
		// if it doesnt exist then a new instance will be created
		template<typeof_singleton_node T> T* get_instance();


		// returns the number of children
		size_t size() const;

		// returns the child node at the given index
		node* at(size_t index);

	private:
		// helper functions
		void __mark_deleted();
		void __remove_child(node* child);
		bool __has_parent(node* node);

		// base properties
		node_tree* m_registry;
		node* m_parent;
		std::vector<node*> m_children;

		// identifing properties
		std::string m_name;
		uint64 m_uid;

		// boolean values
		bool m_active : 1;
		bool m_markedForDeletion : 1;

		friend node_tree;
	};


	// interface for an node that is persistent and can only have one instance
	class singleton_node : public node {
	public:
		singleton_node() : node() {}
		virtual ~singleton_node() = 0 {}
	};


	////// impl node_tree

	inline void node_tree::handle_deletion() {
		for (auto obj : m_markedForDeletion) {
			// remove reference from parent and delete
			if (obj->m_parent) obj->m_parent->__remove_child(obj);
			delete obj;
		}
		m_markedForDeletion.clear();
	}

	inline node_tree::node_tree() : m_uidCounter(0) {
		m_objects.reserve(1000);
	}

	inline node_tree::~node_tree() {
		// delete objects
		for (auto* obj : m_objects) {
			obj->__mark_deleted(); // prevents unintended behaviour
			delete obj;
		}
		m_objects.clear();
		m_markedForDeletion.clear();

		// delete singletons after objects
		for (auto [hash, inst] : m_singletons)
			delete inst;
		m_singletons.clear();

	}

	template<typeof_node T>
	inline T* node_tree::create(node* parent, const std::string& name) {
		// should always be constructable using this method
		// if not constructable then create_method should be overridden to allow construction
		T* obj = be_factory_methods::create_method<T>();

		// setup parent
		if (parent) {
			obj->m_parent = parent;
			parent->m_children.push_back(obj);
		}
		// no parent then its a root node
		else {
			//tree node intoregistry
			if (m_objects.capacity() == m_objects.size()) m_objects.reserve(m_objects.capacity() * 2); // double size
			m_objects.push_back(obj);
		}

		// some more initialization
		obj->set_name(name);
		obj->m_uid = m_uidCounter++;
		obj->m_registry = this;


		// final initialization
		static_cast<node*>(obj)->init(); // cast so we can access it
		return obj;
	}

	template<typeof_singleton_node T>
	inline T* node_tree::get_instance() {
		// check if exists
		auto hash = typehash<T>();
		if (m_singletons.contains(hash))
			return reinterpret_cast<T*>(m_singletons[hash]); // reinterpret because we should be able to garuntee the type using the hash

		// if doesnt exist then we create
		T* single = be_factory_methods::create_method<T>();
		single->m_uid = m_uidCounter++;
		single->m_registry = this;
		m_singletons[hash] = single;
		static_cast<node*>(single)->init();
		return single;
	}

	inline bool node_tree::destroy(node* obj) {
		// marks node to be deleted only if it is not already marked
		if (obj->m_markedForDeletion) return false;

		// check if singleton
		// TODO: see if theres a faster solution 
		if (dynamic_cast<singleton_node*>(obj) != nullptr)
			return false;

		// expand
		if (m_markedForDeletion.capacity() < m_objects.size()) m_markedForDeletion.reserve(m_objects.size() * 2); // double objects

		// mark andregistry
		obj->__mark_deleted();
		m_markedForDeletion.push_back(obj);
		return true;
	}

	inline size_t node_tree::size() const {
		return m_objects.size();
	}

	inline node* node_tree::at(size_t index) {
		return m_objects[index];
	}

	inline void node_tree::__delete(node* obj) {
		// is this node to be removed from the parent?
		if (obj->m_parent && !(obj->m_parent->m_markedForDeletion)) {
			obj->m_parent->__remove_child(obj);
			obj->m_parent = nullptr;
		}

		delete obj;
	}

	inline void node_tree::__remove_marked(node* obj) {
		for (size_t i = 0; i < m_markedForDeletion.size(); ++i) {
			if (m_markedForDeletion[i] == obj) {
				m_markedForDeletion.erase(m_markedForDeletion.begin() + i);
				return;
			}
		}
	}

	inline void node_tree::__add_root(node* o) {
		m_objects.push_back(o);
	}

	inline void node_tree::__remove_root(node* o) {
		for (size_t i = 0; i < m_objects.size(); i++) {
			if (m_objects[i] == o) {
				m_objects.erase(m_objects.begin() + i);
				return;
			}
		}
	}


	////// impl node

	inline node::node()
		: m_registry(nullptr)
		, m_parent(nullptr)
		, m_uid(-1)
		, m_active(true)
		, m_markedForDeletion(false) {}

	inline node::~node() {
		// delete children
		for (size_t i = 0; i < m_children.size(); i++)
			m_registry->__delete(m_children[i]);
		m_children.clear();
	}

	inline node_tree* node::tree() const {
		return m_registry;
	}

	inline node* node::get_parent() const {
		return m_parent;
	}

	template<typeof_node T>
	inline T* node::get_ancestor() const {
		// skip if no parent
		if (m_parent == nullptr) return nullptr;

		// check this
		if (auto* ancestor = dynamic_cast<T*>(m_parent)) return ancestor;

		// recurse
		return m_parent->get_ancestor<T>();
	}

	inline const std::string& node::get_name() const {
		return m_name;
	}

	inline void node::set_name(const std::string& name) {
		m_name = name;
	}

	inline uint64 node::uid() const {
		return m_uid;
	}

	inline bool node::get_active() const {
		return m_active;
	}

	inline void node::set_active(bool active) {
		if (m_active != active) {
			// enable
			if (active) {
				// TODO
			}
			// disable
			else {
				// TODO
			}
		}
		m_active = active;
	}

	inline bool node::destroy(node* obj) {
		return m_registry->destroy(obj);
	}

	template<typeof_node T>
	inline T* node::create(const std::string& name) {
		return m_registry->create<T>(this, name);
	}

	template<typeof_node T>
	inline T* node::create(node* parent, const std::string& name) {
		return m_registry->create<T>(parent, name);
	}

	template<typeof_singleton_node T>
	inline T* node::get_instance() {
		return m_registry->get_instance<T>();
	}

	inline size_t node::size() const {
		return m_children.size();
	}

	inline node* node::at(size_t index) {
		return m_children[index];
	}

	inline void node::__mark_deleted() {
		// if already marked then we remove ourselves as one of the parents will be added 
		if (m_markedForDeletion) {
			m_registry->__remove_marked(this);
			return;
		}

		// mark this and children
		m_markedForDeletion = true;
		for (size_t i = 0; i < m_children.size(); i++) {
			m_children[i]->__mark_deleted();
		}
	}

	inline void node::__remove_child(node* child) {
		for (size_t i = 0; i < m_children.size(); i++) {
			if (m_children[i] == child) {
				m_children.erase(m_children.begin() + i);
				return;
			}
		}
	}

	inline bool node::__has_parent(node* node) {
		if (m_parent == nullptr) return false;
		if (m_parent == node) return true;
		return m_parent->__has_parent(node);
	}

}

#endif // !BE_OBJECT_registry_HPP