#ifndef BE_OBJECT_registry_HPP
#define BE_OBJECT_registry_HPP
#include "common.hpp"
#include <unordered_set>
#include "lua_script.hpp"
#include "reflection\type_hash.hpp"

// the type of integer used for hashing types
// -1 is always reserved as a null type
#define BE_OBJECT_INTEGER uint64

namespace be {

	// types defined in this file
	class object;
	class object_registry;
	class singleton_object; // inherits object

	// concept used to constrain all object template functions
	template<typename T>
	concept typeof_object = std::is_base_of_v<object, T> && !std::is_base_of_v<singleton_object, T>;

	// seperate concept for singleton objects so we dont instance singletons when we shouldnt
	// we also cant instance singleton_object itself
	template<typename T>
	concept typeof_singleton_object = std::is_base_of_v<singleton_object, T> && !std::is_same_v<singleton_object, T>;

	// concept used to constrain all object template functions but doesnt exclude singleton_object
	template<typename T>
	concept typeof_anyobject = std::is_base_of_v<object, T>;

}

// used to override the factory methods
#define be_factory_methods __befactorymethods__

namespace be_factory_methods {

	// create factory method for creating objects
	// can be overridden
	template<be::typeof_anyobject T>
	T* create_method() { return new T(); }

}

namespace be {

	// holds and manages all objects
	class object_registry final { /////////////////////////////////////////////////////////////////////////
	public:
		using channel_type = const char*;
		using listener_type = function<void>;

		// handles deleting objects
		// should be called once per frame
		void handle_deletion();

		// default constructs an object_registry
		object_registry();

		~object_registry();


		// factory method used to construct any object
		template<typeof_object T>
		T* create(object* parent = nullptr, const std::string& name = "");

		// mark an object to be destroyed
		// returns true on success
		// will not destroy singletons
		bool destroy(object* obj);

		// returns a singleton of type
		// if it doesnt exist then a new instance will be created
		template<typeof_singleton_object T> T* get_instance();


		// sends a message on a specific channel
		void message(channel_type channel);


		// returns the number of children
		size_t size() const;

		// returns the child object at the given index
		object* at(size_t index);

	private:
		// helper functions
		void __delete(object* obj);
		void __remove_marked(object* obj);
		void __add_root(object* o);
		void __remove_root(object* o);
		void __add_listener(channel_type channel, listener_type listener);
		void __remove_listener(channel_type channel, listener_type listener);

		// object storage
		std::vector<object*> m_objects;
		std::vector<object*> m_markedForDeletion;
		std::unordered_map<typehash_t, singleton_object*> m_singletons;

		// messaging system
		std::unordered_map<channel_type, event<void>> m_channels;

		// uid counter
		uint64 m_uidCounter;

		friend object;
	};

	// adds basic overrides to a class
	#define BE_OBJECT(o) \
	::be::typehash_t get_hash() const override { return ::be::typehash<o>(); }

	// the based object class
	// can be used as is or inherited from to further develop functionality
	// has built in active toggle which enables and disables message listeners
	class object { ////////////////////////////////////////////////////////////////////////////////////
	public:
		using channel_type = object_registry::channel_type;
		using listener_type = object_registry::listener_type;

		// requires override for any subtypes to allow iobjectfactory to recognize the object during destruction
		// easily overrideable through BE_OBJECT macro
		virtual typehash_t get_hash() const { return typehash<object>(); }

	protected:

		// initialization function called after object is constructed and added to theregistry
		virtual void init() {}

		// called after parent changes
		//virtual void on_reparent(object* oldParent) {}

	public:

		// default constructs object
		// required if not using
		object();

		virtual ~object();


		// returns a reference to theregistry that owns this object
		object_registry*registry() const;

		// returns a reference to the parent object 
		// if there is no parent then it will return nullptr
		object* get_parent() const;

		// returns first ancestor object of type 
		// includes direct parent
		// slow call using dynamic_cast<> at each level
		template<typeof_object T>
		T* get_ancestor() const;

		// tries to reparent this object
		// if this object is marked for deletion or the parent is a child of this object then the call will fail and the function will return false
		//bool set_parent(object* parent);

		// gets the name of this object
		const std::string& get_name() const;

		// sets the name of this object
		void set_name(const std::string& name);

		// gets this objects unique identifier
		uint64 uid() const;

		// gets the activeness of this object
		bool get_active() const;

		// sets the activeness of this object
		void set_active(bool active);


		// creates and attaches an object of type to this object
		template<typeof_object T>
		T* create(const std::string& name = "");

		// creates an object of type and sets the parent
		// if parent is nullptr then it will be created as a root object
		template<typeof_object T>
		T* create(object* parent, const std::string& name = "");

		// mark an object to be destroyed
		// returns true on success
		bool destroy(object* obj);

		// returns a singleton of type
		// if it doesnt exist then a new instance will be created
		template<typeof_singleton_object T> T* get_instance();



	protected:

		// listens for a message
		// can only have one function listening for a channel at a time
		// calling for the same channel will rebind to the new function
		template<auto MemberFunction, typename ClassTy = typename detail::func_sig<decltype(MemberFunction)>::class_ty>
		void listen(channel_type channel);

		// sends a message on a specific channel
		void message(channel_type channel);

	public:

		// loads a lua file and creates a lua environment asocciated with this object
		// returns a reference to the environment
		lua_env* load_script(const char* filepath);

		// returns the environment
		lua_env* get_environment();

		// calls a function on the lua file
		// returns false if unsuccessful
		bool call(const char* func);

		// sets a field within the lua environment
		void set_field(const char* name, const lua_var& value);

		// gets a field from the lua environment
		lua_var get_field(const char* name);



		// returns the number of children
		size_t size() const;

		// returns the child object at the given index
		object* at(size_t index);

	private:
		// helper functions
		void __mark_deleted();
		void __remove_child(object* child);
		bool __has_parent(object* object);

		// base properties
		object_registry* m_registry;
		object* m_parent;
		std::vector<object*> m_children;

		// identifing properties
		std::string m_name;
		uint64 m_uid;

		// boolean values
		bool m_active : 1;
		bool m_markedForDeletion : 1;

		// messaging properties
		std::unordered_map<channel_type, listener_type> m_tunedChannels;

		// lua scripting variables
		lua_env* m_luaEnvironment;

		friend object_registry;
	};


	// interface for an object that is persistent and can only have one instance
	class singleton_object : public object {
	public:
		singleton_object() : object() {}
		virtual ~singleton_object() = 0 {}
	};


	////// impl object_registry

	inline void object_registry::handle_deletion() {
		for (auto obj : m_markedForDeletion) {
			// remove reference from parent and delete
			if (obj->m_parent) obj->m_parent->__remove_child(obj);
			delete obj;
		}
		m_markedForDeletion.clear();
	}

	inline object_registry::object_registry() : m_uidCounter(0) {
		m_objects.reserve(1000);
	}

	inline object_registry::~object_registry() {
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

		// clear everything else
		m_channels.clear();
	}

	template<typeof_object T>
	inline T* object_registry::create(object* parent, const std::string& name) {
		// should always be constructable using this method
		// if not constructable then create_method should be overridden to allow construction
		T* obj = be_factory_methods::create_method<T>();

		// setup parent
		if (parent) {
			obj->m_parent = parent;
			parent->m_children.push_back(obj);
		}
		// no parent then its a root object
		else {
			//registry object intoregistry
			if (m_objects.capacity() == m_objects.size()) m_objects.reserve(m_objects.capacity() * 2); // double size
			m_objects.push_back(obj);
		}

		// some more initialization
		obj->set_name(name);
		obj->m_uid = m_uidCounter++;
		obj->m_registry = this;


		// final initialization
		static_cast<object*>(obj)->init(); // cast so we can access it
		return obj;
	}

	template<typeof_singleton_object T>
	inline T* object_registry::get_instance() {
		// check if exists
		auto hash = typehash<T>();
		if (m_singletons.contains(hash))
			return reinterpret_cast<T*>(m_singletons[hash]); // reinterpret because we should be able to garuntee the type using the hash

		// if doesnt exist then we create
		T* single = be_factory_methods::create_method<T>();
		single->m_uid = m_uidCounter++;
		single->m_registry = this;
		m_singletons[hash] = single;
		return single;
	}

	inline bool object_registry::destroy(object* obj) {
		// marks object to be deleted only if it is not already marked
		if (obj->m_markedForDeletion) return false;

		// check if singleton
		// TODO: see if theres a faster solution 
		if (dynamic_cast<singleton_object*>(obj) != nullptr)
			return false;

		// expand
		if (m_markedForDeletion.capacity() < m_objects.size()) m_markedForDeletion.reserve(m_objects.size() * 2); // double objects

		// mark andregistry
		obj->__mark_deleted();
		m_markedForDeletion.push_back(obj);
		return true;
	}

	inline void object_registry::message(channel_type channel) {
		if (m_channels.contains(channel)) {
			m_channels[channel](); // invoke event
		}
	}

	inline size_t object_registry::size() const {
		return m_objects.size();
	}

	inline object* object_registry::at(size_t index) {
		return m_objects[index];
	}

	inline void object_registry::__delete(object* obj) {
		// is this object to be removed from the parent?
		if (obj->m_parent && !(obj->m_parent->m_markedForDeletion)) {
			obj->m_parent->__remove_child(obj);
			obj->m_parent = nullptr;
		}

		delete obj;
	}

	inline void object_registry::__remove_marked(object* obj) {
		for (size_t i = 0; i < m_markedForDeletion.size(); ++i) {
			if (m_markedForDeletion[i] == obj) {
				m_markedForDeletion.erase(m_markedForDeletion.begin() + i);
				return;
			}
		}
	}

	inline void object_registry::__add_root(object* o) {
		m_objects.push_back(o);
	}

	inline void object_registry::__remove_root(object* o) {
		for (size_t i = 0; i < m_objects.size(); i++) {
			if (m_objects[i] == o) {
				m_objects.erase(m_objects.begin() + i);
				return;
			}
		}
	}

	inline void object_registry::__add_listener(channel_type channel, listener_type listener) {
		m_channels[channel] += listener;
	}

	inline void object_registry::__remove_listener(channel_type channel, listener_type listener) {
		m_channels[channel] -= listener;
	}


	////// impl object

	inline object::object()
		: m_registry(nullptr)
		, m_parent(nullptr)
		, m_uid(-1)
		, m_active(true)
		, m_markedForDeletion(false)
		, m_luaEnvironment(nullptr) {}

	inline object::~object() {
		// delete children
		for (size_t i = 0; i < m_children.size(); i++)
			m_registry->__delete(m_children[i]);
		m_children.clear();

		// remove channels
		for (auto [channel, listener] : m_tunedChannels)
			m_registry->__remove_listener(channel, listener);
		m_tunedChannels.clear();
	}

	inline object_registry* object::registry() const {
		return m_registry;
	}

	inline object* object::get_parent() const {
		return m_parent;
	}

	template<typeof_object T>
	inline T* object::get_ancestor() const {
		// skip if no parent
		if (m_parent == nullptr) return nullptr;

		// check this
		if (auto* ancestor = dynamic_cast<T*>(m_parent)) return ancestor;

		// recurse
		return m_parent->get_ancestor<T>();
	}

	inline const std::string& object::get_name() const {
		return m_name;
	}

	inline void object::set_name(const std::string& name) {
		m_name = name;
	}

	inline uint64 object::uid() const {
		return m_uid;
	}

	inline bool object::get_active() const {
		return m_active;
	}

	inline void object::set_active(bool active) {
		if (m_active != active) {
			// enable
			if (active) {
				for (auto [channel, listener] : m_tunedChannels) {
					m_registry->__add_listener(channel, listener);
				}
			}
			// disable
			else {
				for (auto [channel, listener] : m_tunedChannels) {
					m_registry->__remove_listener(channel, listener);
				}
			}
		}
		m_active = active;
	}

	inline bool object::destroy(object* obj) {
		return m_registry->destroy(obj);
	}

	template<typeof_object T>
	inline T* object::create(const std::string& name) {
		return m_registry->create<T>(this, name);
	}

	template<typeof_object T>
	inline T* object::create(object* parent, const std::string& name) {
		return m_registry->create<T>(parent, name);
	}

	template<typeof_singleton_object T>
	inline T* object::get_instance() {
		return m_registry->get_instance<T>();
	}

	template<auto MemberFunction, typename ClassTy>
	inline void object::listen(channel_type channel) {
		listener_type listener;
		listener.bind<MemberFunction>(reinterpret_cast<ClassTy*>(this));

		// check if channel is alreadyregistryed and should be remapped
		if (m_tunedChannels.contains(channel) && m_tunedChannels[channel] != listener) {
			m_registry->__remove_listener(channel, m_tunedChannels[channel]);
			m_registry->__add_listener(channel, listener);
			m_tunedChannels[channel] = listener;
			return;
		}

		// if not then weregistry for the first time
		m_tunedChannels[channel] = listener;
		m_registry->__add_listener(channel, listener);
	}

	inline void object::message(channel_type channel) {
		m_registry->message(channel);
	}

	inline lua_env* object::load_script(const char* filepath) {
		if (m_luaEnvironment == nullptr) {
			m_luaEnvironment = new lua_env(filepath);
			return m_luaEnvironment;
		}
		return nullptr;
	}

	inline lua_env* object::get_environment() {
		return m_luaEnvironment;
	}

	inline bool object::call(const char* func) {
		if (m_luaEnvironment)
			return m_luaEnvironment->call(func);
		return false;
	}

	inline void object::set_field(const char* name, const lua_var& value) {
		if (m_luaEnvironment)
			m_luaEnvironment->set_field(name, value);
	}

	inline lua_var object::get_field(const char* name) {
		if (m_luaEnvironment)
			return m_luaEnvironment->get_field(name);
		return lua_var(nullptr);
	}

	inline size_t object::size() const {
		return m_children.size();
	}

	inline object* object::at(size_t index) {
		return m_children[index];
	}

	inline void object::__mark_deleted() {
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

	inline void object::__remove_child(object* child) {
		for (size_t i = 0; i < m_children.size(); i++) {
			if (m_children[i] == child) {
				m_children.erase(m_children.begin() + i);
				return;
			}
		}
	}

	inline bool object::__has_parent(object* object) {
		if (m_parent == nullptr) return false;
		if (m_parent == object) return true;
		return m_parent->__has_parent(object);
	}

}

#endif // !BE_OBJECT_registry_HPP