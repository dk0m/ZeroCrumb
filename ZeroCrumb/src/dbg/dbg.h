#pragma once

#include<string>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "../comm/comm.h"

using std::ostringstream;
using std::string;

using std::setw;
using std::hex;
using std::setfill;

namespace dbg {
	string getHexABK(PBYTE key);
}