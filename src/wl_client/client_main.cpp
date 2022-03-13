
#include "VirtualDisplayManager.h"

#include <thread>
#include <tuple>

using namespace android;

int main(int argc, char **argv) {
    std::ignore = argc;
    // std::this_thread::sleep_for(std::chrono::seconds(20));

    // When WLManager is launched in its own process, limit the number of
    // binder threads to 4.
    ProcessState::self()->setThreadPoolMaxThreadCount(4);

    // start the thread pool
    sp<ProcessState> ps(ProcessState::self());

    ps->startThreadPool();

    std::shared_ptr<VirtualDisplayManager> vd = std::make_shared<VirtualDisplayManager>(String8(argv[1]));
    vd->run();

    return 0;
}