#include <Windows.h>
#include <ntsecapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "ntdrvldr.h"

// Command line flags
BOOL bUnload = FALSE;
wchar_t *DriverName = NULL;
wchar_t *DriverPath = NULL;

VOID
PrintErrorAndExit(
    wchar_t *Function,
    ULONG dwErrorCode
    )
{
    LPWSTR Buffer;

    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                   FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL,
                   dwErrorCode,
                   LANG_USER_DEFAULT,
                   (LPWSTR)&Buffer,
                   0,
                   NULL);
    fwprintf(stderr, L"%s: %ws", Function, Buffer);
    exit(dwErrorCode);
}

ULONG
EnablePrivilege(
    LPCWSTR lpPrivilegeName
    )
{
    TOKEN_PRIVILEGES Privilege;
    HANDLE hToken;
    DWORD dwErrorCode;

    Privilege.PrivilegeCount = 1;
    Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
                               &Privilege.Privileges[0].Luid))
        return GetLastError();

    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES, &hToken))
        return GetLastError();

    if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
                               NULL, NULL)) {
        dwErrorCode = GetLastError();
        CloseHandle(hToken);
        return dwErrorCode;
    }

    CloseHandle(hToken);
    return 0;
}

VOID
PrintUsageAndExit(
    VOID
    )
{
    fwprintf(stderr,
         L"Usage: ntdrvldr [-u] [-n DriverName] DriverPath\n");
    exit(1);
}

VOID
ParseCommandLine(
    int argc,
    wchar_t *argv[]
    )
{
    int Index;
    DWORD dwAllocLength;
    DWORD dwLength;
    LPWSTR Buffer;
    LPWSTR FilePart;

    for (Index = 1; Index < argc; ++Index) {
        if (argv[Index][0] != '-') {
            DriverPath = argv[Index];
        } else {
            switch (argv[Index][1]) {
            case 'u':
                bUnload = TRUE;
                break;
            case 'n':
                if (++Index < argc) {
                    DriverName = argv[Index];
                } else {
                    fwprintf(stderr, L"Missing argument for '-n'.\n");
                    exit(1);
                }
                break;
            default:
                if (argv[Index][1] != 'h')
                    fwprintf(stderr, L"Invalid option '%ws'.\n", argv[Index]);
                PrintUsageAndExit();
            }
        }
    }

    if (DriverPath == NULL)
        PrintUsageAndExit();

    // Translate to full path name
    dwAllocLength = GetFullPathNameW(DriverPath, 0, NULL, NULL);
    if (dwAllocLength == 0)
        PrintErrorAndExit(L"GetFullPathNameW", GetLastError());
    Buffer = HeapAlloc(GetProcessHeap(), 0, (dwAllocLength * sizeof(WCHAR)));
    if (Buffer == NULL)
        PrintErrorAndExit(L"HeapAlloc", GetLastError());
    dwLength = GetFullPathNameW(DriverPath, dwAllocLength, Buffer, &FilePart);
    if (dwLength == 0)
        PrintErrorAndExit(L"GetFullPathNameW", GetLastError());
    DriverPath = Buffer;

    // Use filename as driver name if not specified
    if (DriverName == NULL)
        DriverName = FilePart;
}

int
_cdecl
wmain(
    int argc,
    wchar_t *argv[]
    )
{
    ULONG dwErrorCode;

    _wsetlocale(LC_ALL, L"");
    ParseCommandLine(argc, argv);
    dwErrorCode = EnablePrivilege(L"SeLoadDriverPrivilege");
    if (dwErrorCode)
        PrintErrorAndExit(L"EnablePrivilege", dwErrorCode);

    if (!bUnload) {
        dwErrorCode = CreateRegistryKey(DriverName, DriverPath);
        if (dwErrorCode)
            PrintErrorAndExit(L"CreateRegistryKey", dwErrorCode);
        dwErrorCode = LoadDriver(DriverName);
        if (dwErrorCode) {
            DeleteRegistryKey(DriverName);
            PrintErrorAndExit(L"LoadDriver", dwErrorCode);
        }
    } else {
        dwErrorCode = UnloadDriver(DriverName);
        if (dwErrorCode)
            PrintErrorAndExit(L"UnloadDriver", dwErrorCode);
        dwErrorCode = DeleteRegistryKey(DriverName);
        if (dwErrorCode)
            PrintErrorAndExit(L"DeleteRegistryKey", dwErrorCode);
    }
    return 0;
}
