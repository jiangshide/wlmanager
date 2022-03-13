//
// Created by lijing on 2021/8/25.
//

#ifndef WLMANAGER_SRC_WL_CLIENT_WLCONSUMER_H_
#define WLMANAGER_SRC_WL_CLIENT_WLCONSUMER_H_

#include <system/window.h>

#include <gui/BufferQueue.h>
#include <gui/BufferQueueDefs.h>
#include <gui/ConsumerBase.h>

#include <utils/Vector.h>

#include "WaylandWindowManager.h"

class WLConsumer : public android::ConsumerBase {
 public:
    typedef ConsumerBase::FrameAvailableListener FrameAvailableListener;

    struct LockedBuffer {
        uint8_t *data;
        uint32_t width;
        uint32_t height;
        android::PixelFormat format;
        uint32_t stride;
        android::Rect crop;
        uint32_t transform;
        uint32_t scalingMode;
        int64_t timestamp;
        android_dataspace dataSpace;
        uint64_t frameNumber;
        // this is the same as format, except for formats that are compatible with
        // a flexible format (e.g. HAL_PIXEL_FORMAT_YCbCr_420_888). In the latter
        // case this contains that flexible format
        android::PixelFormat flexFormat;
        // Values below are only valid when using HAL_PIXEL_FORMAT_YCbCr_420_888
        // or compatible format, in which case LockedBuffer::data
        // contains the Y channel, and stride is the Y channel stride. For other
        // formats, these will all be 0.
        uint8_t *dataCb;
        uint8_t *dataCr;
        uint32_t chromaStride;
        uint32_t chromaStep;

        LockedBuffer() : data(nullptr),
                         width(0),
                         height(0),
                         format(android::PIXEL_FORMAT_NONE),
                         stride(0),
                         crop(android::Rect::EMPTY_RECT),
                         transform(0),
                         scalingMode(NATIVE_WINDOW_SCALING_MODE_FREEZE),
                         timestamp(0),
                         dataSpace(HAL_DATASPACE_UNKNOWN),
                         frameNumber(0),
                         flexFormat(android::PIXEL_FORMAT_NONE),
                         dataCb(nullptr),
                         dataCr(nullptr),
                         chromaStride(0),
                         chromaStep(0) {}
    };

    WLConsumer(const android::sp<android::IGraphicBufferConsumer> &bq,
               size_t maxLockedBuffers,
               bool isControlledByApp);

    android::status_t lockNextBuffer(LockedBuffer *nativeBuffer);
    android::status_t unlockBuffer(const LockedBuffer &nativeBuffer);
    android::status_t setDefaultBufferSize(uint32_t width, uint32_t height);

    void setWLWindowManager(WaylandWindowManager* wm) {
        m_single_wm = wm;
    }

 private:
    // Maximum number of buffers that can be locked at a time
    const size_t mMaxLockedBuffers;

    // Tracking for buffers acquired by the user
    struct AcquiredBuffer {
        static constexpr uintptr_t kUnusedId = 0;

        // Need to track the original mSlot index and the buffer itself because
        // the mSlot entry may be freed/reused before the acquired buffer is
        // released.
        int mSlot;
        android::sp<android::GraphicBuffer> mGraphicBuffer;
        uintptr_t mLockedBufferId;

        AcquiredBuffer() : mSlot(android::BufferQueue::INVALID_BUFFER_SLOT),
                           mLockedBufferId(kUnusedId) {
        }

        void reset() {
            mSlot = android::BufferQueue::INVALID_BUFFER_SLOT;
            mGraphicBuffer.clear();
            mLockedBufferId = kUnusedId;
        }
    };

    android::status_t lockBufferItem(const android::BufferItem &item, LockedBuffer *outBuffer) const;

    size_t findAcquiredBufferLocked(uintptr_t id) const;

    android::Vector<AcquiredBuffer> mAcquiredBuffers;

    // Count of currently locked buffers
    size_t mCurrentLockedBuffers;

    WaylandWindowManager *m_single_wm;
};

#endif//WLMANAGER_SRC_WL_CLIENT_WLCONSUMER_H_
