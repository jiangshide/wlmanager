#ifndef WL_MANAGER_SERVICE
#define WL_MANAGER_SERVICE

#include <com/jingos/BnBinderWLManagerService.h>
#include <com/jingos/IBinderWLManagerService.h>

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>

#include <binder/AppOpsManager.h>
#include <binder/BinderService.h>
#include <binder/IAppOpsCallback.h>

#include <vector>

#include "MessageQueue.h"
#include <binder/Status.h>

class WLManagerService : public com::jingos::BnBinderWLManagerService,
                         public android::IBinder::DeathRecipient {
 public:
    WLManagerService();
    ~WLManagerService();

    void Run();

    ::android::binder::Status Ping(int32_t i) override;
    ::android::binder::Status StartApp(const android::String16 &name) override;
    ::android::binder::Status DestroyApp(const android::String16 &name) override;
    ::android::binder::Status StopApp(const android::String16 &name) override;

    void binderDied(const android::wp<IBinder> &who);

 private:
    std::unique_ptr<android::MessageQueue> m_queue;
    std::vector<std::string> m_client_list;
};

#endif /* ifndef WL_MANAGER_SERVICE */
