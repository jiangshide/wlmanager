//
// Created by lijing on 2021/8/25.
//

#include "WLConsumer.h"

#include <iostream>

#include <gui/BufferItem.h>
#include <utils/Log.h>

using namespace android;

static uintptr_t getLockedBufferId(const WLConsumer::LockedBuffer &buffer) {
    return reinterpret_cast<uintptr_t>(buffer.data);
}

WLConsumer::WLConsumer(const android::sp<IGraphicBufferConsumer> &bq,
                       size_t maxLockedBuffers,
                       bool isControlledByApp)
    : ConsumerBase(bq, isControlledByApp), mMaxLockedBuffers(maxLockedBuffers) {
    // Create tracking entries for locked buffers
    mAcquiredBuffers.insertAt(0, maxLockedBuffers);

    mConsumer->setMaxAcquiredBufferCount(static_cast<int32_t>(maxLockedBuffers));
    mConsumer->setConsumerUsageBits(GraphicBuffer::USAGE_HW_TEXTURE);
}

status_t WLConsumer::lockNextBuffer(WLConsumer::LockedBuffer *nativeBuffer) {
    status_t err;

    if (!nativeBuffer) return BAD_VALUE;

    BufferItem b;
    err = acquireBufferLocked(&b, 0);
    if (err != OK) {
        if (err == BufferQueue::NO_BUFFER_AVAILABLE) {
            return BAD_VALUE;
        } else {
            std::cout << "Error acquiring buffer: " << strerror(err);
            return err;
        }
    }

    if (b.mGraphicBuffer == nullptr) {
        b.mGraphicBuffer = mSlots[b.mSlot].mGraphicBuffer;
    }

    err = lockBufferItem(b, nativeBuffer);
    if (err != OK) {
        return err;
    }

    // find an unused AcquiredBuffer
    size_t lockedIdx = findAcquiredBufferLocked(AcquiredBuffer::kUnusedId);
    ALOG_ASSERT(lockedIdx < mMaxLockedBuffers);
    AcquiredBuffer &ab = mAcquiredBuffers.editItemAt(lockedIdx);

    ab.mSlot = b.mSlot;
    ab.mGraphicBuffer = b.mGraphicBuffer;
    ab.mLockedBufferId = getLockedBufferId(*nativeBuffer);

    mCurrentLockedBuffers++;

    return OK;
}

status_t WLConsumer::unlockBuffer(const WLConsumer::LockedBuffer &nativeBuffer) {
    Mutex::Autolock _l(mMutex);

    uintptr_t id = getLockedBufferId(nativeBuffer);
    size_t lockedIdx =
        (id != AcquiredBuffer::kUnusedId) ? findAcquiredBufferLocked(id) : mMaxLockedBuffers;
    if (lockedIdx == mMaxLockedBuffers) {
        std::cout << __func__ << ": Can't find buffer to free";
        return BAD_VALUE;
    }

    AcquiredBuffer &ab = mAcquiredBuffers.editItemAt(lockedIdx);

    int fenceFd = -1;
    status_t err = ab.mGraphicBuffer->unlockAsync(&fenceFd);
    if (err != OK) {
        std::cout << __func__ << "Unable to unlock graphic buffer " << lockedIdx;
        return err;
    }

    sp<Fence> fence(fenceFd >= 0 ? new Fence(fenceFd) : Fence::NO_FENCE);
    addReleaseFenceLocked(ab.mSlot, ab.mGraphicBuffer, fence);
    releaseBufferLocked(ab.mSlot, ab.mGraphicBuffer);

    ab.reset();

    mCurrentLockedBuffers--;

    return OK;
}

status_t WLConsumer::lockBufferItem(const BufferItem &item, WLConsumer::LockedBuffer *outBuffer) const {
    PixelFormat format = item.mGraphicBuffer->getPixelFormat();
    PixelFormat flexFormat = format;

    void *bufferPointer = nullptr;
    int fenceFd = item.mFence.get() ? item.mFence->dup() : -1;
    status_t err = item.mGraphicBuffer->lockAsync(GraphicBuffer::USAGE_SW_READ_OFTEN,
                                                  item.mCrop, &bufferPointer, fenceFd);
    if (err != OK) {
        std::cout << "Unable to lock buffer for CPU reading: " << strerror(-err);
        return err;
    }

    std::vector<std::string> layer_names;
    const sp<GraphicBuffer> target = item.mGraphicBuffer;
    m_single_wm->pushFrame(layer_names, target->handle, target->width, target->height, target->stride, target->format);

    outBuffer->data = reinterpret_cast<uint8_t *>(bufferPointer);
    outBuffer->stride = item.mGraphicBuffer->getStride();
    outBuffer->dataCb = nullptr;
    outBuffer->dataCr = nullptr;
    outBuffer->chromaStride = 0;
    outBuffer->chromaStep = 0;

    outBuffer->width = item.mGraphicBuffer->getWidth();
    outBuffer->height = item.mGraphicBuffer->getHeight();
    outBuffer->format = format;
    outBuffer->flexFormat = flexFormat;

    outBuffer->crop = item.mCrop;
    outBuffer->transform = item.mTransform;
    outBuffer->scalingMode = item.mScalingMode;
    outBuffer->timestamp = item.mTimestamp;
    outBuffer->dataSpace = item.mDataSpace;
    outBuffer->frameNumber = item.mFrameNumber;
    return NO_ERROR;
}

size_t WLConsumer::findAcquiredBufferLocked(uintptr_t id) const {
    for (size_t i = 0; i < mMaxLockedBuffers; i++) {
        const auto &ab = mAcquiredBuffers[i];
        // note that this finds AcquiredBuffer::kUnusedId as well
        if (ab.mLockedBufferId == id) {
            return i;
        }
    }
    return mMaxLockedBuffers; // an invalid index
}

status_t WLConsumer::setDefaultBufferSize(uint32_t width, uint32_t height) {
    Mutex::Autolock lock(mMutex);
    return mConsumer->setDefaultBufferSize(width, height);
}
