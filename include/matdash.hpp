#pragma once
#include "matdash/detail/impl.hpp"
#include <utility>

namespace matdash {
	inline void add_hook_impl(void* addr, void* detour, void** trampoline);

	template <auto func, class T>
	void add_hook(const T address) {
		using F = detail::clean_fn_type<decltype(func)>::type;
		using R = detail::function_ret<F>::type;
		using Info = detail::extract_cc_or<R, CallConv::Membercall>;
		add_hook<func, Info::value>(address);
	}

	template <auto func, CallConv conv, class T>
	void add_hook(const T address) {
		const auto addr = reinterpret_cast<void*>(address);
		static constexpr auto callable = detail::ternary<std::is_member_function_pointer_v<decltype(func)>>::value< 
			&detail::wrap_member_fn<decltype(func)>::wrap<func>,
			func>;
		using F = typename detail::clean_fn_type<decltype(func)>::type;
		using F1 = typename detail::remove_value_wrapper<F>::type;
		void* func_addr;
		void** tramp_addr;

		using wrapper = typename detail::wrapper_for_cc<conv>::type<F1>;

		func_addr = reinterpret_cast<void*>(&wrapper::template wrap<callable>);
		tramp_addr = &wrapper::tramp<func>;

		add_hook_impl(addr, func_addr, tramp_addr);
	}

	template <auto func, class... Args>
	decltype(auto) orig(Args&&... args) {
		using F = typename detail::clean_fn_type<decltype(func)>::type;
		using R = typename detail::function_ret<F>::type;
		using Info = typename detail::extract_cc_or<R, CallConv::Membercall>;
		return orig<func, Info::value>(args...);
	}

	template <auto func, CallConv conv, class... Args>
	decltype(auto) orig(Args&&... args) {
		using F = typename detail::clean_fn_type<decltype(func)>::type;
		using F1 = typename detail::remove_value_wrapper<F>::type;

		using wrapper = typename detail::wrapper_for_cc<conv>::type<F1>;

		return wrapper::invoke<func>(std::forward<Args>(args)...);
	}
}
