//
// Created by lijing on 2021/8/21.
//

#ifndef WLMANAGER_SRC_VIRTUALDISPLAYMANAGER_H_
#define WLMANAGER_SRC_VIRTUALDISPLAYMANAGER_H_

#include <binder/IPCThreadState.h>
#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayInfo.h>

#include "WLFrameOutput.h"
#include "WaylandSingleWindowManager.h"

class VirtualDisplayManager {
 public:
    VirtualDisplayManager(const android::String8 &screenName);
    ~VirtualDisplayManager();

    android::status_t prepareVirtualDisplay(const android::DisplayInfo &mainDpyInfo,
                                            const android::sp<android::IGraphicBufferProducer> &bufferProducer,
                                            android::sp<android::IBinder> *pDisplayHandle,
                                            const android::String8 &screenName);

    android::status_t setDisplayProjection(android::SurfaceComposerClient::Transaction &t,
                                           const android::sp<android::IBinder> &dpy,
                                           const android::DisplayInfo &mainDpyInfo);

    void run();

 private:
    WaylandWindowManager* m_single_wm;
    android::sp<WLFrameOutput> wlFrameOutput;
};

#endif//WLMANAGER_SRC_VIRTUALDISPLAYMANAGER_H_
