#include "pch.h"
#include "FSHooksLib.h"
#include "global.h"

typedef void (*callback_function)(int);
callback_function g_callback = nullptr;
HHOOK g_hook = nullptr;

extern "C" FSHOOKS_API void Init()
{
    if (g_log)
    {
        g_log.close();
    }

    g_log.open("hookslog.txt", std::ofstream::out);
    g_log << "FSHooksLib has been initialized" << std::endl;
}

extern "C" FSHOOKS_API void Release()
{
    if (g_log)
    {
        g_log << "FSHooksLib has been releaseded" << std::endl;
        g_log.close();
    }
}

extern "C" FSHOOKS_API void SetCallback(callback_function callback)
{
    g_callback = callback;
    g_log << "Callback has been set" << std::endl;
}

extern "C" FSHOOKS_API LRESULT HookProc(int code, WPARAM wParam, LPARAM lParam)
{
    if (code != HC_ACTION)
        return CallNextHookEx(nullptr, code, wParam, lParam);

    MSLLHOOKSTRUCT* info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    POINT point = info->pt;
    DWORD flags = info->flags;
    bool injected = flags & LLMHF_INJECTED;
    bool lower_injected = flags & LLMHF_LOWER_IL_INJECTED;
    DWORD time = info->time;

    switch (wParam)
    {
    case WM_LBUTTONDOWN:
        g_log << "Received a WM_LBUTTONDOWN message with injected=" << injected << std::endl;
        
        if (g_callback)
        {
            g_callback(g_numProcesses);
        }
        break;
    case WM_LBUTTONUP:
        g_log << "Received a WM_LBUTTONUP message with injected=" << injected << std::endl;
        break;
    default:
        break;
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}
