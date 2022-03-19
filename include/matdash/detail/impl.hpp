#pragma once
#include <utility>

namespace matdash {
	namespace cc {
		namespace {
			template <class T>
			struct ValueWrapper {
				T value;
				template <class... Args>
				ValueWrapper(Args&&... v) : value(std::forward<Args>(v)...) {}
				operator T(){ return value; }
			};

			template <>
			struct ValueWrapper<void> {};
		}

		template <class T>
		struct optcall : ValueWrapper<T> {};

		template <class T>
		struct membercall : ValueWrapper<T> {};

		template <class T>
		struct thiscall : ValueWrapper<T> {};

		// cant name it cdecl :(
		template <class T>
		struct c_decl : ValueWrapper<T> {};
	}

	enum CallConv {
		Optcall,
		Membercall,
		Thiscall,
		Cdecl
	};

	namespace detail {
		template <class F>
		struct function_ret;

		template <class R, class... Args>
		struct function_ret<R(Args...)> {
			using type = R;
		};

		template <class T, CallConv cc>
		struct extract_cc_or {
			static constexpr auto value = cc;
			using type = T;
		};

		template <class T, auto a>
		struct extract_cc_or<cc::optcall<T>, a> {
			static constexpr auto value = CallConv::Optcall;
			using type = T;
		};

		template <class T, auto a>
		struct extract_cc_or<cc::membercall<T>, a> {
			static constexpr auto value = CallConv::Membercall;
			using type = T;
		};

		template <class T, auto a>
		struct extract_cc_or<cc::thiscall<T>, a> {
			static constexpr auto value = CallConv::Thiscall;
			using type = T;
		};

		template <class T, auto a>
		struct extract_cc_or<cc::c_decl<T>, a> {
			static constexpr auto value = CallConv::Cdecl;
			using type = T;
		};

		template <class F>
		struct wrap_member_fn {
			template <auto>
			static void wrap() {}
		};

		template <class R, class T, class... Args>
		struct wrap_member_fn<R(T::*)(Args...)> {
			template <auto func>
			static R wrap(T* self, Args... args) {
				return (self->*func)(args...);
			}
		};

		template <class F>
		struct clean_fn_type { using type = F; };

		template <class R, class... Args>
		struct clean_fn_type<R(*)(Args...)> {
			using type = R(Args...);
		};

		template <class R, class T, class... Args>
		struct clean_fn_type<R(T::*)(Args...)> {
			using type = R(T*, Args...);
		};

		template <class F>
		struct remove_value_wrapper;

		template <class R, class... Args>
		struct remove_value_wrapper<R(Args...)> {
			using ret = typename extract_cc_or<R, CallConv::Cdecl>::type;
			using type = ret(Args...);
		};

		template <bool>
		struct ternary {
			template <auto a, auto b>
			static constexpr auto value = a;
		};

		template <>
		struct ternary<false> {
			template <auto a, auto b>
			static constexpr auto value = b;
		};

		namespace {
			template <class T>
			struct _print_type {
				static_assert(!std::is_same_v<T, T>, "_print_type");
			};
		}

		template <class T>
		constexpr void print_type_debug() {
			_print_type<T> a{};
		}

		namespace wrappers {
			template <class F>
			struct thiscall;

			template <class R, class... Args>
			struct thiscall<R(Args...)> {
				template <auto func>
				static R __thiscall wrap(Args... args) {
					if constexpr (std::is_same_v<R, void>)
						func(args...);
					else
						return func(args...);
				}
				template <auto>
				static void* tramp;
				template <auto func>
				static R invoke(Args... args) {
					return reinterpret_cast<R(__thiscall*)(Args...)>(tramp<func>)(args...);
				}
			};

			template <class F>
			struct c_decl;

			template <class R, class... Args>
			struct c_decl<R(Args...)> {
				template <auto func>
				static R __cdecl wrap(Args... args) {
					if constexpr (std::is_same_v<R, void>)
						func(args...);
					else
						return func(args...);
				}
				template <auto>
				static void* tramp;
				template <auto func>
				static R invoke(Args... args) {
					return reinterpret_cast<R(__cdecl*)(Args...)>(tramp<func>)(args...);
				}
			};

			template <class F>
			struct membercall {
				template <auto>
				static void* tramp;
			};

			template <class F>
			struct optcall {
				template <auto>
				static void* tramp;
			};

			#include "gen.hpp"
		}

		template <CallConv conv>
		struct wrapper_for_cc {
			static_assert(conv == conv, "unknown cc");
		};

		template <>
		struct wrapper_for_cc<CallConv::Thiscall> {
			template <class T>
			using type = wrappers::thiscall<T>;
		};

		template <>
		struct wrapper_for_cc<CallConv::Cdecl> {
			template <class T>
			using type = wrappers::c_decl<T>;
		};

		template <>
		struct wrapper_for_cc<CallConv::Membercall> {
			template <class T>
			using type = wrappers::membercall<T>;
		};

		template <>
		struct wrapper_for_cc<CallConv::Optcall> {
			template <class T>
			using type = wrappers::optcall<T>;
		};
	}
}