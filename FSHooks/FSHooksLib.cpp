#include "pch.h"
#include "FSHooksLib.h"
#include "global.h"

extern "C" FSHOOKS_API void Init()
{
    g_log.open("hookslog.txt", std::ofstream::out);
    g_log << "FSHooksLib has been initialized" << std::endl;
}

extern "C" FSHOOKS_API void Release()
{
    g_log << "FSHooksLib has been releaseded" << std::endl;
    g_log.close();
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
        break;
    case WM_LBUTTONUP:
        g_log << "Received a WM_LBUTTONUP message with injected=" << injected << std::endl;
        break;
    default:
        break;
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

