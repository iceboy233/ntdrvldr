// Should include ntdef.h or (Windows.h and ntsecapi.h) before this file

#ifndef _NTDRVLDR_H_
#define _NTDRVLDR_H_

ULONG
StringConcatUnicode(
    PUNICODE_STRING ResultString,
    LPWSTR FirstString,
    LPWSTR SecondString
    );

ULONG
StringConcat(
    LPWSTR *ResultString,
    LPWSTR FirstString,
    LPWSTR SecondString
    );

VOID
StringFreeUnicode(
    PUNICODE_STRING UnicodeString
    );

VOID
StringFree(
    LPWSTR String
    );

ULONG
CreateRegistryKey(
    const LPWSTR DriverName,
    const LPWSTR DriverPath
    );

ULONG
DeleteRegistryKey(
    const LPWSTR DriverName
    );

ULONG
LoadDriver(
    const LPWSTR DriverName
    );

ULONG
UnloadDriver(
    const LPWSTR DriverName
    );

#endif
