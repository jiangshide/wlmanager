//
// Created by lijing on 2021/8/21.
//

#include "VirtualDisplayManager.h"
#include "WLFrameOutput.h"

#include <thread>
#include <utils/Log.h>

using namespace android;

static bool gRotate = false;    // rotate 90 degrees
static uint32_t gVideoWidth = 0;// default width+height
static uint32_t gVideoHeight = 0;

static inline uint32_t floorToEven(uint32_t num) {
    return num & ~1;
}

VirtualDisplayManager::VirtualDisplayManager(const String8 &screenName) {
    status_t err;

    m_single_wm = new WaylandSingleWindowManager();
    m_single_wm->initialize();

    // Start Binder thread pool.  MediaCodec needs to be able to receive
    // messages from mediaserver.
    sp<ProcessState> self = ProcessState::self();
    self->startThreadPool();

    // Get main display parameters.
    sp<IBinder> mainDpy = SurfaceComposerClient::getInternalDisplayToken();
    while (mainDpy == nullptr) {
        ALOGE("get main display token failed");
        sleep(1);
        mainDpy = SurfaceComposerClient::getInternalDisplayToken();
    }

    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(mainDpy, &mainDpyInfo);
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to get display characteristics\n");
        return;
    }

    // TODO: 确定需要吗？
    // Encoder can't take odd number as config
    if (gVideoWidth == 0) {
        gVideoWidth = floorToEven(mainDpyInfo.viewportW);
    }
    if (gVideoHeight == 0) {
        gVideoHeight = floorToEven(mainDpyInfo.viewportH);
    }

    sp<IGraphicBufferProducer> inputSurface;
    wlFrameOutput = new WLFrameOutput();
    wlFrameOutput->createInputSurface(gVideoWidth, gVideoHeight, &inputSurface);
    wlFrameOutput->setWLWindowManager(m_single_wm);

    sp<IBinder> dpy;
    prepareVirtualDisplay(mainDpyInfo, inputSurface, &dpy, screenName);
}

VirtualDisplayManager::~VirtualDisplayManager() {
    m_single_wm->finalize();
}

status_t VirtualDisplayManager::prepareVirtualDisplay(const DisplayInfo &mainDpyInfo,
                                                      const sp<IGraphicBufferProducer> &bufferProducer,
                                                      sp<IBinder> *pDisplayHandle, const String8 &screenName) {
    std::cout << "prepareVirtualDisplay ===============\n";
    // TODO: 名字由包名确定
    sp<IBinder> dpy = SurfaceComposerClient::createDisplay(screenName, false /*secure*/);

    SurfaceComposerClient::Transaction t;
    t.setDisplaySurface(dpy, bufferProducer);
    setDisplayProjection(t, dpy, mainDpyInfo);
    t.setDisplayLayerStack(dpy, 0);// default stack
    t.apply();

    *pDisplayHandle = dpy;

    return NO_ERROR;
}

/*
 * Sets the display projection, based on the display dimensions, video size,
 * and device orientation.
 */
status_t VirtualDisplayManager::setDisplayProjection(
    SurfaceComposerClient::Transaction &t,
    const sp<IBinder> &dpy,
    const DisplayInfo &mainDpyInfo) {

    // Set the region of the layer stack we're interested in, which in our
    // case is "all of it".
    Rect layerStackRect(mainDpyInfo.viewportW, mainDpyInfo.viewportH);

    // We need to preserve the aspect ratio of the display.
    float displayAspect = (float) mainDpyInfo.viewportH / (float) mainDpyInfo.viewportW;

    // Set the way we map the output onto the display surface (which will
    // be e.g. 1280x720 for a 720p video).  The rect is interpreted
    // post-rotation, so if the display is rotated 90 degrees we need to
    // "pre-rotate" it by flipping width/height, so that the orientation
    // adjustment changes it back.
    //
    // We might want to encode a portrait display as landscape to use more
    // of the screen real estate.  (If players respect a 90-degree rotation
    // hint, we can essentially get a 720x1280 video instead of 1280x720.)
    // In that case, we swap the configured video width/height and then
    // supply a rotation value to the display projection.
    uint32_t videoWidth, videoHeight;
    uint32_t outWidth, outHeight;
    if (!gRotate) {
        videoWidth = gVideoWidth;
        videoHeight = gVideoHeight;
    } else {
        videoWidth = gVideoHeight;
        videoHeight = gVideoWidth;
    }
    if (videoHeight > (uint32_t) (videoWidth * displayAspect)) {
        // limited by narrow width; reduce height
        outWidth = videoWidth;
        outHeight = (uint32_t) (videoWidth * displayAspect);
    } else {
        // limited by short height; restrict width
        outHeight = videoHeight;
        outWidth = (uint32_t) (videoHeight / displayAspect);
    }
    uint32_t offX, offY;
    offX = (videoWidth - outWidth) / 2;
    offY = (videoHeight - outHeight) / 2;
    Rect displayRect(offX, offY, offX + outWidth, offY + outHeight);

    std::cout << __func__ << ": width: " << outWidth << "\n";

    t.setDisplayProjection(dpy,
                           gRotate ? DISPLAY_ORIENTATION_90 : DISPLAY_ORIENTATION_0,
                           layerStackRect, displayRect);
    return NO_ERROR;
}

void VirtualDisplayManager::run() {
    while (true) {
        wlFrameOutput->inputBufferLocked();
    }
}
