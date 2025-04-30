#pragma once
#include<windows.h>

namespace comm {
	constexpr DWORD keySize = 32;
	HANDLE connectToPipe();
	PBYTE readAppBoundKey(HANDLE hPipe);
}