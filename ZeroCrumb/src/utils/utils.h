#pragma once

#include<windows.h>
#include<string>

using std::string;

namespace utils {
	string getEnvVar(LPCSTR name);
}