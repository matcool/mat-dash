#pragma once
#include "../src/template-utils.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Minhook.h>
#include <gd.h>

static bool __hooks_initialized = false;

template <auto func, typename CallConv = Optcall>
void add_hook(uintptr_t address) {
    if (!__hooks_initialized) {
        MH_Initialize();
        __hooks_initialized = true;
    }
    if constexpr (std::is_same_v<CallConv, Optcall>) {
        if constexpr (std::is_member_function_pointer_v<decltype(func)>) {
            MH_CreateHook(
                reinterpret_cast<void*>(address),
                &optcall<RemoveThiscall<MemberToFn<decltype(func)>::type>::type>::wrap<WrapMemberCall<func>::wrap>,
                reinterpret_cast<void**>(&Orig<func, Optcall>::orig)
            );
        } else {
            MH_CreateHook(
                reinterpret_cast<void*>(address),
                &optcall<decltype(func)>::wrap<func>,
                reinterpret_cast<void**>(&Orig<func, Optcall>::orig)
            );
        }
    } else if constexpr (std::is_same_v<CallConv, Thiscall>) {
        MH_CreateHook(
            reinterpret_cast<void*>(address),
            &thiscall<decltype(func)>::wrap<func>,
            reinterpret_cast<void**>(&Orig<func, Thiscall>::orig)
        );
    } else {
        static_assert(std::false_type::value, "Invalid calling convention");
    }
    MH_EnableHook(reinterpret_cast<void*>(address));
}