// TestDLL.cpp
#include <windows.h>
#include "pch.h"

#include <winerror.h>
#include <psapi.h>

void Attack() {
    char szProcessName[128];
    GetModuleBaseNameA(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
    MessageBoxA(NULL, "DLL Injected Successfully!", szProcessName, MB_OK);
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        Attack();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}