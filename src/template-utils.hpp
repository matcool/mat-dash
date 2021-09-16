#pragma once
#include <type_traits>

template <class F>
struct optcall;

template <auto func>
struct WrapMemberCall {
    template <typename T, typename... Args>
    static auto wrap(T* instance, Args... args) {
        return (instance->*func)(args...);
    }
};

template <typename R, typename... Args>
struct optcall<R(*)(Args...)> : public optcall<R(Args...)> {};

template<class R, typename T, class... Args>
struct optcall<R(T, float, Args...)> {
    using F = R(T, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, Args... args) {
        float a1;
        __asm movss a1, xmm1;
        return func(self, a1, args...);
    }
};

template<class R, typename T, typename A, class... Args>
struct optcall<R(T, A, float, Args...)> {
    using F = R(T, A, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, A a1, Args... args) {
        float a2;
        __asm movss a2, xmm2;
        return func(self, a1, a2, args...);
    }
};

template<class R, typename T, class... Args>
struct optcall<R(T, float, float, Args...)> {
    using F = R(T, float, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, Args... args) {
        float a1, a2;
        __asm movss a1, xmm1
        __asm movss a2, xmm2
        return func(self, a1, a2, args...);
    }
};

template<class R, typename T, typename A, typename B, class... Args>
struct optcall<R(T, A, B, float, Args...)> {
    using F = R(T, A, B, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, A a1, B a2, Args... args) {
        float a3;
        __asm movss a3, xmm3
        return func(self, a1, a2, a3, args...);
    }
};

template<class R, typename T, typename B, class... Args>
struct optcall<R(T, float, B, float, Args...)> {
    using F = R(T, float, B, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, B a2, Args... args) {
        float a1, a3;
        __asm movss a1, xmm1
        __asm movss a3, xmm3
        return func(self, a1, a2, a3, args...);
    }
};

template<class R, typename T, typename A, class... Args>
struct optcall<R(T, A, float, float, Args...)> {
    using F = R(T, A, float, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, A a1, Args... args) {
        float a2, a3;
        __asm movss a2, xmm2
        __asm movss a3, xmm3
        return func(self, a1, a2, a3, args...);
    }
};

template<class R, typename T, class... Args>
struct optcall<R(T, float, float, float, Args...)> {
    using F = R(T, float, float, float, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, Args... args) {
        float a1, a2, a3;
        __asm movss a1, xmm1
        __asm movss a2, xmm2
        __asm movss a3, xmm3
        return func(self, a1, a2, a3, args...);
    }
};

template<class R, typename T, class... Args>
struct optcall<R(T, Args...)> {
    using F = R(T, Args...); 
    template <F func>
    static R __fastcall wrap(T self, void*, Args... args) { return func(self, args...); }
};


template<class F>
struct WrapperOptcall;

template <typename R, typename... Args>
struct WrapperOptcall<R(*)(Args...)> : public WrapperOptcall<R(__thiscall*)(Args...)> {};

template <typename R, typename A, typename... Args>
struct WrapperOptcall<R(__thiscall*)(A, float, Args...)> {
    R(__thiscall* addr)(A, Args...);
    auto operator()(A a0, float a1, Args... args) {
        __asm movss xmm1, a1;
        return addr(a0, args...);
    }
};

template <typename R, typename A, typename B, typename... Args>
struct WrapperOptcall<R(__thiscall*)(A, B, float, Args...)> {
    R(__thiscall* addr)(A, B, Args...);
    auto operator()(A a0, B a1, float a2, Args... args) {
        __asm movss xmm2, a2;
        return addr(a0, a1, args...);
    }
};

// TODO: copy paste more to complete 2**4 specializations of WrapperOptcall

template<typename R, typename... Args>
struct WrapperOptcall<R(__thiscall*)(Args...)> {
    R(__thiscall* addr)(Args...);
    auto operator()(Args... args) {
        return addr(args...);
    }
};

template <typename F>
struct MemberToFn;

template <typename R, typename T, typename... Args>
struct MemberToFn<R(T::*)(Args...)> {
    using type = typename R(__thiscall*)(T*, Args...);
};

template <typename F>
struct RemoveThiscall;

template <typename R, typename... Args>
struct RemoveThiscall<R(__thiscall*)(Args...)> {
    using type = typename R(Args...);
};

template <typename F>
struct AddThiscall;

template <typename R, typename... Args>
struct AddThiscall<R(Args...)> {
    using type = typename R(__thiscall*)(Args...);
};

template <typename R, typename... Args>
struct AddThiscall<R(*)(Args...)> {
    using type = typename R(__thiscall*)(Args...);
};

// to be used in template args
struct Optcall;
struct Thiscall;

template <auto func, typename CallConv, typename enable = void>
struct Orig {
    inline static WrapperOptcall<decltype(func)> orig;
};

template <auto func, typename CallConv>
struct Orig<func, CallConv, std::enable_if_t<std::is_member_function_pointer<decltype(func)>::value> > {
    inline static WrapperOptcall<typename MemberToFn<decltype(func)>::type> orig;
};

template <auto func>
struct Orig<func, Thiscall, void> {
    inline static typename AddThiscall<decltype(func)>::type orig;
};

template <auto func, typename CallConv = Optcall, typename... Args>
auto orig(Args... args) {
    return Orig<func, CallConv>::orig(args...);
}

template <typename F>
struct thiscall;

template<class R, class... Args>
struct thiscall<R(*)(Args...)> : public thiscall<R(Args...)> {};

template<class R, typename T, class... Args>
struct thiscall<R(T, Args...)> {
    using F = R(T, Args...);
    template <F func>
    static R __fastcall wrap(T self, void*, Args... args) { return func(self, args...); }
};