#include <windows.h>

#include <iostream>

int main()
{
    HMODULE hMod = LoadLibrary("FSHooks.dll");
    if (!hMod)
    {
        std::cerr << "Count not load the FSHooks library" << std::endl;
        return -1;
    }

    FARPROC fn_init = GetProcAddress(hMod, "Init");
    if (!fn_init)
    {
        std::cerr << "Count not get the 'Init' function from FSHooks library" << std::endl;
        return -1;
    }

    FARPROC fn_release = GetProcAddress(hMod, "Release");
    if (!fn_release)
    {
        std::cerr << "Count not get the 'Release' function from FSHooks library" << std::endl;
        return -1;
    }

    HOOKPROC fn_hook = reinterpret_cast<HOOKPROC>(GetProcAddress(hMod, "HookProc"));
    if (!fn_hook)
    {
        std::cerr << "Count not get the 'HookProc' function from FSHooks library" << std::endl;
        return -1;
    }

    HHOOK hook = SetWindowsHookEx(WH_MOUSE_LL, fn_hook, hMod, 0);
    if (!hook)
    {
        std::cerr << "Failed to SetWindowsHookEx" << std::endl;
        return -1;
    }

    fn_init();

    MSG msg;
    // TODO - We'll need some manner of signal to terminate
    //        This is just a quick and dirty example program
    while (true)
    {
        GetMessage(&msg, nullptr, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    fn_release();
    FreeLibrary(hMod);
    return 0;
}
