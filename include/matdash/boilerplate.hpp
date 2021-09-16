template <typename F>
struct __mat_dash_function_arg_count;

template <typename R, typename... Args>
struct __mat_dash_function_arg_count<R(Args...)> {
    static constexpr auto count = sizeof...(Args);
};

DWORD WINAPI __mat_dash_thread_function(void* module) {
    // if constexpr (__mat_dash_function_arg_count<decltype(mod_main)>::count == 1)
    //     mod_main(reinterpret_cast<HMODULE>(module));
    // else
        mod_main();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, __mat_dash_thread_function, module, 0, 0);
    return TRUE;
}