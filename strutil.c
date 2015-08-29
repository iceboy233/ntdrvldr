#include <Windows.h>
#include <ntsecapi.h>
#include "ntdrvldr.h"

ULONG
StringConcatUnicode(
    PUNICODE_STRING ResultString,
    LPWSTR FirstString,
    LPWSTR SecondString
    )
{
    SIZE_T FirstStringSize = lstrlenW(FirstString) * sizeof(WCHAR);
    SIZE_T SecondStringSize = lstrlenW(SecondString) * sizeof(WCHAR);
    SIZE_T TotalSize = FirstStringSize + SecondStringSize;
    LPWSTR Buffer;

    if (TotalSize > 0xFFFF)
        return ERROR_BUFFER_OVERFLOW;

    Buffer = HeapAlloc(GetProcessHeap(), 0, TotalSize);
    if (Buffer == NULL)
        return GetLastError();

    RtlCopyMemory(Buffer, FirstString, FirstStringSize);
    RtlCopyMemory((LPVOID)((ULONG_PTR)Buffer + FirstStringSize),
                  SecondString, SecondStringSize);
    ResultString->Length = (USHORT)TotalSize;
    ResultString->MaximumLength = (USHORT)TotalSize;
    ResultString->Buffer = Buffer;
    return 0;
}

ULONG
StringConcat(
    LPWSTR *ResultString,
    LPWSTR FirstString,
    LPWSTR SecondString
    )
{
    SIZE_T FirstStringSize = lstrlenW(FirstString) * sizeof(WCHAR);
    SIZE_T SecondStringSize = lstrlenW(SecondString) * sizeof(WCHAR);
    SIZE_T AllocSize = FirstStringSize + SecondStringSize + sizeof(WCHAR);
    LPWSTR Buffer;

    Buffer = HeapAlloc(GetProcessHeap(), 0, AllocSize);
    if (Buffer == NULL)
        return GetLastError();

    RtlCopyMemory(Buffer, FirstString, FirstStringSize);
    RtlCopyMemory((LPVOID)((ULONG_PTR)Buffer + FirstStringSize),
                  SecondString,
                  SecondStringSize + sizeof(WCHAR));
    *ResultString = Buffer;
    return 0;
}

VOID
StringFreeUnicode(
    PUNICODE_STRING UnicodeString
    )
{
    HeapFree(GetProcessHeap(), 0, UnicodeString->Buffer);
}

VOID
StringFree(
    LPWSTR String
    )
{
    HeapFree(GetProcessHeap(), 0, String);
}
