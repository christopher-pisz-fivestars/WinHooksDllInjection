#include "pch.h"
#include "FSHooksLib.h"
#include "global.h"

#include <chrono>

using namespace std::chrono;

typedef void (*callback_function)(int, int, int,bool);
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
        g_log << "FSHooksLib has been released" << std::endl;
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
    
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    MSLLHOOKSTRUCT * info = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    /*
    POINT point = info->pt;
    DWORD flags = info->flags;
    bool injected = flags & LLMHF_INJECTED;
    bool lower_injected = flags & LLMHF_LOWER_IL_INJECTED;
    DWORD time = info->time;
    */

    switch (wParam)
    {
    case WM_LBUTTONDOWN:
        if (g_callback)
        {
            g_callback(WM_LBUTTONDOWN, info->pt.x, info->pt.y, info->flags & LLMHF_INJECTED);
        }
        break;
    case WM_LBUTTONUP:
        if (g_callback)
        {
            g_callback(WM_LBUTTONUP, info->pt.x, info->pt.y, info->flags & LLMHF_INJECTED);
        }
        break;
    default:
        break;
    }

    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> delta_time = duration_cast<duration<double>>(t2 - t1);
    g_log << "Callback took " << delta_time.count() << " seconds" <<std::endl;

    return CallNextHookEx(nullptr, code, wParam, lParam);
}
