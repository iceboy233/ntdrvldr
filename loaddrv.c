#include <ntdef.h>
#include "ntdrvldr.h"

#define DRIVER_SERVICE_NAME_PREFIX L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\"

NTSYSAPI
NTSTATUS
NtLoadDriver(
    PUNICODE_STRING DriverServiceName
    );

NTSYSAPI
NTSTATUS
NtUnloadDriver(
    PUNICODE_STRING DriverServiceName
    );

NTSYSAPI
ULONG
RtlNtStatusToDosError(
    NTSTATUS Status
    );

ULONG
LoadDriver(
    const LPWSTR DriverName
    )
{
    UNICODE_STRING DriverServiceName;
    ULONG dwErrorCode;
    NTSTATUS Status;

    dwErrorCode = StringConcatUnicode(&DriverServiceName,
                                      DRIVER_SERVICE_NAME_PREFIX, DriverName);
    if (dwErrorCode)
        return dwErrorCode;

    Status = NtLoadDriver(&DriverServiceName);
    StringFreeUnicode(&DriverServiceName);
    if (!NT_SUCCESS(Status))
        return RtlNtStatusToDosError(Status);

    return 0;
}

ULONG
UnloadDriver(
    const LPWSTR DriverName
    )
{
    UNICODE_STRING DriverServiceName;
    ULONG dwErrorCode;
    NTSTATUS Status;

    dwErrorCode = StringConcatUnicode(&DriverServiceName,
                                      DRIVER_SERVICE_NAME_PREFIX, DriverName);
    if (dwErrorCode)
        return dwErrorCode;

    Status = NtUnloadDriver(&DriverServiceName);
    StringFreeUnicode(&DriverServiceName);
    if (!NT_SUCCESS(Status))
        return RtlNtStatusToDosError(Status);

    return 0;
}
