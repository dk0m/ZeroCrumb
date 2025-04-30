#include "comm.h"

HANDLE comm::connectToPipe() {
    return CreateFile(
        L"\\\\.\\pipe\\ZeroCrumb",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
}

PBYTE comm::readAppBoundKey(HANDLE hPipe) {
    BYTE key[keySize];
    DWORD bytesRead = 0;

    BOOL result = ReadFile(
        hPipe,
        key,
        keySize,
        &bytesRead,
        NULL
    );

    if (!result || bytesRead == 0) {
        return NULL;
    }
    else {
        return key;
    }
}