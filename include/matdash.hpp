#pragma once
#include "../src/template-utils.hpp"

void __mat_dash_add_hook(void* addr, void* detour, void** trampoline);

namespace {
    template <auto>
    constexpr const bool __stupid_false = false;
}

template <auto func, typename CallConv = Optcall>
void add_hook(uintptr_t address) {
    const auto addr = reinterpret_cast<void*>(address);
    if constexpr (std::is_same_v<CallConv, Optcall>) {
        if constexpr (std::is_member_function_pointer_v<decltype(func)>) {
            __mat_dash_add_hook(addr,
                reinterpret_cast<void*>(&optcall<typename RemoveThiscall<typename MemberToFn<decltype(func)>::type>::type>::template wrap<WrapMemberCall<func>::template wrap>),
                reinterpret_cast<void**>(&Orig<func, Optcall>::orig)
            );
        } else {
            __mat_dash_add_hook(addr,
                reinterpret_cast<void*>(&optcall<decltype(func)>::template wrap<func>),
                reinterpret_cast<void**>(&Orig<func, Optcall>::orig)
            );
        }
    } else if constexpr (std::is_same_v<CallConv, Thiscall>) {
        __mat_dash_add_hook(addr,
            reinterpret_cast<void*>(&thiscall<decltype(func)>::template wrap<func>),
            reinterpret_cast<void**>(&Orig<func, Thiscall>::orig)
        );
    } else if constexpr (std::is_same_v<CallConv, Optfastcall>) {
        __mat_dash_add_hook(addr,
            reinterpret_cast<void*>(&optfastcall<decltype(func)>::template wrap<func>),
            reinterpret_cast<void**>(&Orig<func, CallConv>::orig)
        );
    } else {
        static_assert(__stupid_false<func>, "Invalid calling convention");
    }
}

template <auto func, typename CallConv = Optcall>
void add_hook(void* address) {
    add_hook<func, CallConv>(reinterpret_cast<uintptr_t>(address));
}

template <typename R, typename T, typename... Args>
void* member_addr(R(T::* func)(Args...)) {
    return reinterpret_cast<void*&>(func);
}