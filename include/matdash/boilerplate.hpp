template <typename F>
struct __mat_dash_main_wrapper;

template <>
struct __mat_dash_main_wrapper<void(HMODULE)> {
    template <auto func>
    static void call(HMODULE module) { func(module); }
};

template <>
struct __mat_dash_main_wrapper<void()> {
    template <auto func>
    static void call(HMODULE) { func(); }
};

DWORD WINAPI __mat_dash_thread_function(void* module) {
    __mat_dash_main_wrapper<decltype(mod_main)>::call<mod_main>(reinterpret_cast<HMODULE>(module));
    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, __mat_dash_thread_function, module, 0, 0);
    return TRUE;
}