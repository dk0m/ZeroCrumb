#include "dbg.h"

string dbg::getHexABK(PBYTE key) {
    ostringstream oss;
    for (size_t i = 0; i < comm::keySize; ++i) {
        oss << hex << setw(2) << setfill('0') << static_cast<int>(key[i]);
    }
    return oss.str();
}