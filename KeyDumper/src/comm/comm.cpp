#include "comm.h"

HANDLE comm::createPipe() {
	return CreateNamedPipe(
        L"\\\\.\\pipe\\ZeroCrumb",
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE,
        1,
        0,
        0,
        0,
        NULL
    );
}

BOOL comm::waitUntilConnected(HANDLE hPipe) {
    return ConnectNamedPipe(hPipe, NULL);
}

BOOL comm::writeAppBoundKey(HANDLE hPipe, PBYTE key) {

    BOOL result = WriteFile(
        hPipe,
        key,
        comm::keySize,
        NULL,
        NULL
    );

    if (result) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}