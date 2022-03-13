
#include "WLManagerService.h"

#include <binder/IPCThreadState.h>
#include <iostream>
#include <tuple>
#include <log/log.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include <vector>

using namespace android;

WLManagerService::WLManagerService() : m_queue(std::make_unique<impl::MessageQueue>()) {
    m_queue->init();
}

WLManagerService::~WLManagerService() {
}

::android::binder::Status WLManagerService::Ping(int32_t i) {
    std::cout << "ping i: " << i << "\n";

    return ::android::binder::Status::ok();
}

void WLManagerService::binderDied(const android::wp<IBinder> &who) {
    std::ignore = who;
}

::android::binder::Status WLManagerService::StartApp(const String16& name) {
    std::cout << "start application name: " << String8(name) << "\n";

    if (String8(name).find("jingnote.MainActivity#0") == std::string::npos) {
        return ::android::binder::Status::ok();
    }

    bool found = (std::find(m_client_list.begin(), m_client_list.end(), String8(name).c_str()) != m_client_list.end());
    if (!found) {
        m_client_list.push_back(String8(name).c_str());
    } else {
        return ::android::binder::Status::ok();
    }

    pid_t pid = fork();
    if (pid == 0) {
        // child process
        std::cout << "fork!\n";

        const std::vector<std::string> cmdline{ "wl_client", String8(name).c_str() };
        std::vector<const char*> argv;

        for ( const auto& s : cmdline ) {
            argv.push_back( s.data() );
        }
        argv.push_back(NULL);
        argv.shrink_to_fit();
        execv("/system/bin/wl_client", const_cast<char* const *>(argv.data()));
    }

    return ::android::binder::Status::ok();
}

::android::binder::Status WLManagerService::DestroyApp(const String16& name) {

    std::ignore = name;
    return ::android::binder::Status::ok();
}

::android::binder::Status WLManagerService::StopApp(const String16& name) {

    std::ignore = name;
    return ::android::binder::Status::ok();
}

void WLManagerService::Run() {
    while (true) {
        m_queue->waitMessage();
    }
}
