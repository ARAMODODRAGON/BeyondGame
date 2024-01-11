#ifndef BE_CORE_SIGNAL_HPP
#define BE_CORE_SIGNAL_HPP
#include "function.hpp"
#include <unordered_map>
#include <set>

namespace be {

	// decl
	class receiver;

	namespace detail {
		class isignal {
		public:
			virtual ~isignal() = 0 {}
			virtual void __remove(receiver* r) = 0;
		};
	}

	// signal class
	template<typename Return, typename... Arguments>
	class signal final : public detail::isignal {
	public:
		using function_ty = function<void, Arguments...>;

		signal() = default;

		~signal();

		// emits this signal 
		void emit(Arguments... args);

		// emits this signal and handles the return value
		void emit(auto callback, Arguments... args);

	private:
		std::unordered_map<receiver*, function_ty> m_recievers;
	public:
		void __remove(receiver* r) override;
		void __add(receiver* r, function_ty& func);
	};

	// template constraint for signals
	template<typename T>
	concept typeof_signal = std::is_base_of_v<detail::isignal, T>;

	// signal receiver
	class receiver final {
	public:

		receiver() = default;

		~receiver();

		// subscribes to a signal
		// if already subscribed, overwrites the last subscription
		template<auto Function, typeof_signal S>
		void subscribe(S& signal);

		// subscribes to a signal
		// if already subscribed, overwrites the last subscription
		template<auto MemberFunction, typename Class, typeof_signal S>
		void subscribe(Class* object, S& signal);

	private:
		std::set<detail::isignal*> m_signals;
	public:
		template<typeof_signal S>
		void __remove(S* s);
	};

	// impl

	template<typename Return, typename ...Arguments>
	inline signal<Return, Arguments...>::~signal() {
		for (auto& [r, f] : m_recievers)
			r->__remove(this);
		m_recievers.clear();
	}

	template<typename Return, typename ...Arguments>
	inline void signal<Return, Arguments...>::emit(Arguments... args) {
		for (auto& [r, f] : m_recievers)
			f(args...);
	}

	template<typename Return, typename ...Arguments>
	inline void signal<Return, Arguments...>::emit(auto callback, Arguments... args) {
		for (auto& [r, f] : m_recievers)
			callback(f(args...));
	}

	template<typename Return, typename ...Arguments>
	inline void signal<Return, Arguments...>::__remove(receiver* r) {
		m_recievers.erase(r);
	}

	template<typename Return, typename ...Arguments>
	inline void signal<Return, Arguments...>::__add(receiver* r, function_ty& func) {
		m_recievers.emplace(r, func);
	}


	inline receiver::~receiver() {
		for (auto* sig : m_signals)
			sig->__remove(this);
		m_signals.clear();
	}

	template<auto Function, typeof_signal S>
	inline void receiver::subscribe(S& signal) {
		if (m_signals.contains(&signal))
			signal.__remove(this);
		auto func = make_function<Function>();
		signal.__add(this, func);
		m_signals.insert(&signal);
	}

	template<auto MemberFunction, typename Class, typeof_signal S>
	inline void receiver::subscribe(Class* object, S& signal) {
		if (m_signals.contains(&signal))
			signal.__remove(this);
		auto func = make_function<MemberFunction>(object);
		signal.__add(this, func);
		m_signals.insert(&signal);
	}

	template<typeof_signal S>
	inline void receiver::__remove(S* s) {
		m_signals.erase(s);
	}

}

#endif // !BE_CORE_SIGNAL_HPP