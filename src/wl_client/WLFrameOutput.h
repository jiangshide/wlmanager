#ifndef WLMANAGER_SRC_WLFRAMEOUTPUT_H_
#define WLMANAGER_SRC_WLFRAMEOUTPUT_H_

#include <condition_variable>
#include <gui/BufferQueue.h>
#include <gui/CpuConsumer.h>
#include <gui/GLConsumer.h>
#include <iostream>
#include <mutex>

#include "EglWindow.h"
#include "Program.h"
#include "WLConsumer.h"
#include "WaylandWindowManager.h"

// #define GL
// #define OUTPUT_DEBUG

class WLFrameOutput : public android::CpuConsumer::FrameAvailableListener {
 public:
    WLFrameOutput();
    android::status_t createInputSurface(int width, int height,
                                         android::sp<android::IGraphicBufferProducer> *pBufferProducer);
    void inputBufferLocked();
    void setWLWindowManager(WaylandWindowManager *wm) {
        mWLConsumer->setWLWindowManager(wm);
    }

    // Destruction via RefBase.
    virtual ~WLFrameOutput() {
        std::cout << "ddddddddddddddddddddddddddddd\n";
#ifdef GL
        delete[] mPixelBuf;
#endif
    }

 private:
    WLFrameOutput(const WLFrameOutput &);
    WLFrameOutput &operator=(const WLFrameOutput &);

    // (overrides GLConsumer::FrameAvailableListener method)
    virtual void onFrameAvailable(const android::BufferItem &item);

    bool mFrameAvailable = false;
#ifdef GL
    android::sp<android::GLConsumer> mGlConsumer;
    // GL rendering support.
    android::Program mExtTexProgram;
    // External texture, updated by GLConsumer.
    GLuint mExtTextureName;
    // EGL display / context / surface.
    android::EglWindow mEglWindow;

    // Pixel data buffer.
    uint8_t *mPixelBuf;
#else
    android::sp<WLConsumer> mWLConsumer;
#endif

    // Used to wait for the FrameAvailableListener callback.
    android::Mutex mMutex;
    android::Condition mEventCond;

    WaylandWindowManager *m_single_wm;

#ifdef OUTPUT_DEBUG
    FILE *rawFp = nullptr;
#endif
};

#endif//WLMANAGER_SRC_WLFRAMEOUTPUT_H_
