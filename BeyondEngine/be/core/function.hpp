/*
* MIT License
*
* Copyright (c) 2023 Domara Shlimon
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef BOE_FUNCTION_HPP
#define BOE_FUNCTION_HPP
#include <tuple>
#include <memory>
#include <list>

namespace be {

	// template declaration 
	template<typename _RetTy, typename... ArgsTy>
	struct function;

	namespace detail {

		template<typename Sig>
		struct func_sig;

		template<typename RetTy, typename... ArgsTy>
		struct func_sig<RetTy(*)(ArgsTy...)> {
			using sig_t = RetTy(*)(ArgsTy...);
			using ret_t = RetTy;
			using class_ty = void;
		};

		template<typename RetTy, typename ClassTy, typename... ArgsTy>
		struct func_sig<RetTy(ClassTy::*)(ArgsTy...)> {
			using sig_t = RetTy(ClassTy::*)(ArgsTy...);
			using simple_t = RetTy(*)(ArgsTy...);
			using ret_ty = RetTy;
			using class_ty = ClassTy;
		};

		template<typename Sig>
		struct get_function;

		template<typename Ret, typename... Args>
		struct get_function<Ret(*)(Args...)> {
			using type = function<Ret, Args...>;
		};

		template<typename Ret, typename... Args>
		struct get_function<Ret(Args...)> {
			using type = function<Ret, Args...>;
		};

		template<typename Ret, typename Class, typename... Args>
		struct get_function<Ret(Class::*)(Args...)> {
			using type = function<Ret, Args...>;
		};

		template<auto Fun>
		using get_function_t = typename get_function<decltype(Fun)>::type;

	}

	// template definition
	template<typename _RetTy, typename... ArgsTy>
	struct function {

		using returnty = _RetTy;
		using funcsigty = returnty(*)(ArgsTy...);
		using arguments = std::tuple<ArgsTy...>;

		constexpr function(std::nullptr_t = nullptr)
			: m_instance(nullptr), m_function(nullptr), m_invokeable(nullptr) { }

		template<typename T>
		function(T lambda)
			: m_instance(nullptr), m_function(), m_invokeable(nullptr) {
			bind(lambda);
		}

		operator bool() const {
			return m_instance != nullptr || m_function != nullptr || m_invokeable.get() != nullptr;
		}

		bool operator==(const function& other) const {
			if (m_instance)		 return this->m_instance == other.m_instance && this->m_memberbinding == other.m_memberbinding;
			else if (m_function) return this->m_function == other.m_function;
			else				 return this->m_invokeable.get() == other.m_invokeable.get();
		}

		bool operator!=(const function& other) const {
			return !operator==(other);
		}

		void unbind() {
			m_invokeable.reset();
			m_instance = nullptr;
			m_function = nullptr;
			m_memberbinding = nullptr;
		}

		template<funcsigty Function>
		void bind() {
			m_invokeable.reset();
			m_instance = nullptr;
			m_memberbinding = nullptr;
			m_function = Function;
		}

		template<typename T> 
		void bind(T lambda) {
			m_instance = nullptr;
			m_memberbinding = nullptr;
			m_function = nullptr;
			if constexpr (std::is_convertible_v<T, funcsigty>) // easily converted lambda
				m_function = lambda;
			else  // heap allocated invokable object
				m_invokeable.reset(new invokeable<T>(lambda));
		}

		template<auto Member, typename ClassTy = typename detail::func_sig<decltype(Member)>::class_ty>
		void bind(ClassTy* instance) {
			m_invokeable.reset();
			m_instance = instance;
			m_function = nullptr;
			m_memberbinding = [](void* ptr, ArgsTy... args) { // binding lambda
				return (reinterpret_cast<ClassTy*>(ptr)->*Member)(args...);
			};
		}

		returnty operator()(ArgsTy... args) const {
			// invoke member function
			if (m_instance)			return m_memberbinding(m_instance, args...);
			// invoke non-member function
			else if (m_function)	return m_function(args...);
			// invoke pointer to invokeable
			else					return m_invokeable->invoke(args...);
		}

		void* instance() const {
			return m_instance;
		}

	private:

		struct _invokable {
			virtual returnty invoke(ArgsTy...) = 0;
		};
		template<typename ObjectTy>
		struct invokeable : _invokable {
			ObjectTy object;
			returnty invoke(ArgsTy... args) override {
				object(args...);
			}
			invokeable(ObjectTy& object_) : object(object_) { }
		};
		template<typename ObjectTy>
		struct invokeable<ObjectTy&> : _invokable {
			ObjectTy& object;
			returnty invoke(ArgsTy... args) override {
				object(args...);
			}
			invokeable(ObjectTy& object_) : object(object_) { }
		};
		template<typename ObjectTy>
		struct invokeable<ObjectTy*> : _invokable {
			ObjectTy* object;
			returnty invoke(ArgsTy... args) override {
				(*object)(args...);
			}
			invokeable(ObjectTy* object_) : object(object_) { }
		};

		void* m_instance;
		union {
			returnty(*m_memberbinding)(void*, ArgsTy...);
			funcsigty m_function;
		};
		std::shared_ptr<_invokable> m_invokeable;
	};

	template<auto Function, typename function = detail::get_function_t<Function>>
	function make_function() {
		function f;
		f.bind<Function>();
		return f;
	}

	template<auto Member, typename ClassTy, typename function = detail::get_function_t<Member>>
	function make_function(ClassTy* instance) {
		function f;
		f.bind<Member>(instance);
		return f;
	}

	template<auto Lambda, typename... ArgsHint, typename function = function<ArgsHint...>>
	function make_function() {
		function f;
		f.bind<Lambda>();
		return f;
	}

	template<typename... ArgsHint, typename function = function<ArgsHint...>, typename Lambda>
	constexpr function make_function(Lambda lambda) {
		return function(lambda);
	}

	template<typename _RetTy, typename... ArgsTy>
	struct event {
		using returnty = _RetTy;
		using funcsigty = returnty(*)(ArgsTy...);
		using function = function<_RetTy, ArgsTy...>;
		using arguments = std::tuple<ArgsTy...>;

		event() { }

		event& operator+=(const function& f) {
			for (auto& func : m_listeners) {
				if (func == f) {
					return *this;
				}
			}
			m_listeners.push_back(f);
			return *this;
		}

		event& operator-=(const function& f) {
			m_listeners.remove_if([f](auto& func) { return func == f; });
			return *this;
		}

		event& operator-=(const void* instance) {
			m_listeners.remove_if([instance](auto& func) { return func.instance() == instance; });
			return *this;
		}

		void operator()(ArgsTy... args) const {
			for (auto& func : m_listeners) func(args...);
		}

		template<typename T>
		void operator()(T returncallback, ArgsTy... args) {
			for (auto& func : m_listeners) returncallback(func(args...));
		}

		void clear() {
			m_listeners.clear();
		}

	private:
		std::list<function> m_listeners;
	};

}

#endif // !BOE_FUNCTION_HPP