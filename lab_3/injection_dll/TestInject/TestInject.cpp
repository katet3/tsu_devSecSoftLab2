// InjectDLL.cpp
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>

using namespace std;

DWORD GetProcessIdByName(const wstring& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            if (wcscmp(pe.szExeFile, processName.c_str()) == 0) {
                CloseHandle(hSnapshot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return 0;
}

int main() {
    wstring dllPath = L"C:\\Path\\To\\TestDLL.dll"; // Укажите путь к вашей DLL
    wstring targetProcessName = L"notepad.exe"; // Целевой процесс

    DWORD pid = GetProcessIdByName(targetProcessName);
    if (pid == 0) {
        cerr << "Target process not found!" << endl;
        return -1;
    }

    cout << "Target Process ID: " << pid << endl;

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        cerr << "Failed to open target process!" << endl;
        return -1;
    }

    void* pRemoteMemory = VirtualAllocEx(hProcess, NULL, (dllPath.size() + 1) * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
    if (!pRemoteMemory) {
        cerr << "Failed to allocate memory in target process!" << endl;
        CloseHandle(hProcess);
        return -1;
    }

    if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath.c_str(), (dllPath.size() + 1) * sizeof(wchar_t), NULL)) {
        cerr << "Failed to write DLL path to target process memory!" << endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }

    FARPROC pLoadLibrary = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
    if (!pLoadLibrary) {
        cerr << "Failed to get address of LoadLibraryW!" << endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pLoadLibrary, pRemoteMemory, 0, NULL);
    if (!hThread) {
        cerr << "Failed to create remote thread!" << endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    }

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    cout << "DLL injected successfully!" << endl;
    Sleep(5000);

    return 0;
}