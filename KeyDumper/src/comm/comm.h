#pragma once
#include<windows.h>

namespace comm {
	constexpr DWORD keySize = 32;

	HANDLE createPipe();
	BOOL waitUntilConnected(HANDLE hPipe);
	BOOL writeAppBoundKey(HANDLE hPipe, PBYTE key);
}