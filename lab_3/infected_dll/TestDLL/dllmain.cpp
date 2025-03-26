// TestDLL.cpp
#include <windows.h>
#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        // Проверяем возможность создания GUI
        if (AttachThreadInput(GetCurrentThreadId(), GetWindowThreadProcessId(GetForegroundWindow(), NULL), TRUE)) {
            MessageBox(NULL, L"TestDLL was successfully injected!", L"Info", MB_OK);
        }
        else {
            OutputDebugString(L"GUI not available!");
        }
        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}