
#include <binder/IPCThreadState.h>
#include <tuple>

#include "WLManagerService.h"

using namespace android;

int main(int argc, char *argv[]) {
    std::ignore = argc;
    std::ignore = argv;

    // When WLManager is launched in its own process, limit the number of
    // binder threads to 4.
    ProcessState::self()->setThreadPoolMaxThreadCount(4);

    // start the thread pool
    sp<ProcessState> ps(ProcessState::self());
    sp<IServiceManager> sm = defaultServiceManager();

    WLManagerService* wl_service = new WLManagerService();
    sm->addService(String16("wlmanagerservice"), wl_service);

    ps->startThreadPool();
    // IPCThreadState::self()->joinThreadPool();

    // TODO
    // setpriority(PRIO_PROCESS, 0, PRIORITY_URGENT_DISPLAY);
    // set_sched_policy(0, SP_FOREGROUND);

    wl_service->Run();

    return 0;
}

