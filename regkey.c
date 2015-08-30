#include <Windows.h>
#include <ntsecapi.h>
#include <Shlwapi.h>
#include "ntdrvldr.h"

#define REGISTRY_PATH_PREFIX L"System\\CurrentControlSet\\Services\\"
#define SERVICE_IMAGE_PATH_PREFIX L"\\??\\"

ULONG
CreateRegistryKey(
    const LPWSTR DriverName,
    const LPWSTR DriverPath
    )
{
    LPWSTR RegistryPath;
    ULONG dwErrorCode;
    HKEY hKey;
    DWORD dwDisposition;
    DWORD dwServiceType = 1;
    DWORD dwServiceErrorControl = 1;
    DWORD dwServiceStart = 3;
    LPWSTR ServiceImagePath;
    SIZE_T ServiceImagePathSize;

    dwErrorCode = StringConcat(&RegistryPath,
                               REGISTRY_PATH_PREFIX, DriverName);
    if (dwErrorCode)
        return dwErrorCode;

    dwErrorCode = StringConcat(&ServiceImagePath,
                               SERVICE_IMAGE_PATH_PREFIX, DriverPath);
    if (dwErrorCode) {
        StringFree(RegistryPath);
        return dwErrorCode;
    }

    dwErrorCode = RegCreateKeyExW(HKEY_LOCAL_MACHINE,
                                  RegistryPath,
                                  0,
                                  NULL,
                                  0,
                                  KEY_ALL_ACCESS,
                                  NULL,
                                  &hKey,
                                  &dwDisposition);
    StringFree(RegistryPath);
    if (dwErrorCode) {
        StringFree(ServiceImagePath);
        return dwErrorCode;
    }
    if (dwDisposition != REG_CREATED_NEW_KEY) {
        RegCloseKey(hKey);
        return ERROR_ALREADY_EXISTS;
    }

    ServiceImagePathSize = (lstrlenW(ServiceImagePath) + 1) * sizeof(WCHAR);
    dwErrorCode = RegSetValueExW(hKey,
                                 L"ImagePath",
                                 0,
                                 REG_EXPAND_SZ,
                                 (const BYTE *)ServiceImagePath,
                                 ServiceImagePathSize);
    StringFree(ServiceImagePath);
    if (dwErrorCode) {
        RegCloseKey(hKey);
        return dwErrorCode;
    }

    dwErrorCode = RegSetValueExW(hKey,
                                 L"Type",
                                 0,
                                 REG_DWORD,
                                 (const BYTE *)&dwServiceType,
                                 sizeof(DWORD));
    if (dwErrorCode) {
        RegCloseKey(hKey);
        return dwErrorCode;
    }

    dwErrorCode = RegSetValueExW(hKey,
                                 L"ErrorControl",
                                 0,
                                 REG_DWORD,
                                 (const BYTE *)&dwServiceErrorControl,
                                 sizeof(DWORD));
    if (dwErrorCode) {
        RegCloseKey(hKey);
        return dwErrorCode;
    }

    dwErrorCode = RegSetValueExW(hKey,
                                 L"Start",
                                 0,
                                 REG_DWORD,
                                 (const BYTE *)&dwServiceStart,
                                 sizeof(DWORD));
    RegCloseKey(hKey);
    return dwErrorCode;
}

ULONG
DeleteRegistryKey(
    const LPWSTR DriverName
    )
{
    LPWSTR RegistryPath;
    ULONG dwErrorCode;

    dwErrorCode = StringConcat(&RegistryPath, REGISTRY_PATH_PREFIX, DriverName);
    if (dwErrorCode)
        return GetLastError();

    dwErrorCode = SHDeleteKeyW(HKEY_LOCAL_MACHINE, RegistryPath);
    StringFree(RegistryPath);
    return dwErrorCode;
}
