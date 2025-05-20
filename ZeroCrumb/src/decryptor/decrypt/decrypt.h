#pragma once
#include<sodium.h>
#include<string>
#include<vector>
#include<windows.h>

using std::string;
using std::vector;

namespace decryptor {

	static string defaultResult = "N/A";

	BOOL initSodium();
	std::string decrypt(PBYTE key, std::vector<BYTE>& ciphertext);
}