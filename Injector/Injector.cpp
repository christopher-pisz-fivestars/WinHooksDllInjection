#include <windows.h>

int main()
{
    HMODULE hMod = LoadLibrary("FSHooks.dll");
    FARPROC fn_init = GetProcAddress(hMod, "Init");
    FARPROC fn_release = GetProcAddress(hMod, "Release");
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)GetProcAddress(hMod, "HookProc"), hMod, 0);

    MSG msg;
    while (true)
    {
        GetMessage(&msg, nullptr, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    FreeLibrary(hMod);
    return 0;
}
