#include <Windows.h>
#include <iostream>
#include <string>
#include <psapi.h>

// Необходимо для линковки с Psapi.lib
#pragma comment(lib, "psapi.lib")

#define PROTECTION_ARG    "protected_mode"
#define NO_PROTECTION_ARG "no_protected_mode"

// Функция для создания защищенного процесса
BOOL CreateProtectedProcess(LPSTR lpCommandLine, DWORD* dwProcessId, HANDLE* hProcess, HANDLE* hThread) {
    STARTUPINFOEXA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    SIZE_T attrSize = 0;

    si.StartupInfo.cb = sizeof(STARTUPINFOEXA);
    si.StartupInfo.dwFlags = EXTENDED_STARTUPINFO_PRESENT;

    // Инициализация атрибутов
    InitializeProcThreadAttributeList(NULL, 1, 0, &attrSize);
    auto pAttrList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), 0, attrSize);
    InitializeProcThreadAttributeList(pAttrList, 1, 0, &attrSize);

    // Установка политики блокировки DLL
    DWORD64 policy = PROCESS_CREATION_MITIGATION_POLICY_BLOCK_NON_MICROSOFT_BINARIES_ALWAYS_ON;
    UpdateProcThreadAttribute(pAttrList, 0, PROC_THREAD_ATTRIBUTE_MITIGATION_POLICY,
        &policy, sizeof(policy), NULL, NULL);

    si.lpAttributeList = pAttrList;

    // Создание защищенного процесса
    BOOL result = CreateProcessA(
        NULL,
        lpCommandLine,
        NULL,
        NULL,
        FALSE,
        EXTENDED_STARTUPINFO_PRESENT | CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si.StartupInfo,
        &pi
    );

    if (result) {
        *dwProcessId = pi.dwProcessId;
        *hProcess = pi.hProcess;
        *hThread = pi.hThread;
    }

    // Очистка
    DeleteProcThreadAttributeList(pAttrList);
    HeapFree(GetProcessHeap(), 0, pAttrList);

    return result;
}

// Основная рабочая функция
void MainWorkFunction() {
    int counter = 0;
    while (true) {
        std::cout << "Counter: " << counter++ << std::endl;
        Sleep(1000);

        // Дополнительная проверка загруженных модулей
        /*
        if (counter % 10 == 0) {
            HMODULE hMods[1024];
            DWORD cbNeeded;
            if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded)) {
                std::cout << "Loaded modules count: " << (cbNeeded / sizeof(HMODULE)) << std::endl;
            }
        }
        */
    }
}

int main(int argc, char* argv[]) {
    // Проверяем, запущен ли процесс в защищенном режиме
    if (argc == 2 && std::string(argv[1]) == PROTECTION_ARG) {
        std::cout << "Running in protected mode. DLL injection blocked." << std::endl;
        MainWorkFunction();
    }
    else if (argc == 2 && std::string(argv[1]) == NO_PROTECTION_ARG) {
        std::cout << "Running no protected mode. DLL injection no blocked." << std::endl;
        MainWorkFunction();
    }
    else {
        // Если нет, перезапускаем себя с защитой
        std::cout << "Restarting in protected mode..." << std::endl;

        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);

        std::string cmdLine = std::string(path) + " " + PROTECTION_ARG;

        DWORD pid;
        HANDLE hProcess, hThread;
        if (CreateProtectedProcess(const_cast<char*>(cmdLine.c_str()), &pid, &hProcess, &hThread)) {
            std::cout << "Protected process started with PID: " << pid << std::endl;
            CloseHandle(hProcess);
            CloseHandle(hThread);
        }
        else {
            std::cerr << "Failed to create protected process. Error: " << GetLastError() << std::endl;
            return 1;
        }
    }

    return 0;
}