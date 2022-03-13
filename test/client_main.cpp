
#include <tuple>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <utils/Errors.h>
#include <stdio.h>
#include <vector>
#include <iostream>

#include <com/jingos/IBinderWLManagerService.h>

using namespace android;

int main(int argc, char** argv) {
    std::ignore = argc;
    std::ignore = argv;

    sp<com::jingos::IBinderWLManagerService> wls = interface_cast<com::jingos::IBinderWLManagerService>(defaultServiceManager()->getService(String16("wlmanagerservice")));
    wls->StartApp(String16("test_screen0"));

    return 0;
}