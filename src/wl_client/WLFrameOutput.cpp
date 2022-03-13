//
// Created by lijing on 2021/8/23.
//

#define ATRACE_TAG ATRACE_TAG_GRAPHICS
#include "WLFrameOutput.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <tuple>
#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <utils/Trace.h>

using namespace android;

#ifdef GL
static const int kGlBytesPerPixel = 4;      // GL_RGBA
//static const int kOutBytesPerPixel = 3;     // RGB only
#endif

//std::mutex mMutex;
//std::condition_variable mCv;

#ifdef OUTPUT_DEBUG
static const int kOutBytesPerPixel = 3;     // RGB only
static void reduceRgbaToRgb(uint8_t *buf, unsigned int pixelCount) {
    // Convert RGBA to RGB.
    //
    // Unaligned 32-bit accesses are allowed on ARM, so we could do this
    // with 32-bit copies advancing at different rates (taking care at the
    // end to not go one byte over).
    const uint8_t *readPtr = buf;
    for (unsigned int i = 0; i < pixelCount; i++) {
        *buf++ = *readPtr++;
        *buf++ = *readPtr++;
        *buf++ = *readPtr++;
        readPtr++;
    }
}

static FILE *prepareRawOutput(const char *fileName) {
    FILE *rawFp = NULL;

    if (strcmp(fileName, "-") == 0) {
        fprintf(stderr, "ERROR: verbose output and '-' not compatible");
        return NULL;
        rawFp = stdout;
    } else {
        rawFp = fopen(fileName, "w");
        if (rawFp == NULL) {
            fprintf(stderr, "fopen raw failed: %s\n", strerror(errno));
            return NULL;
        }
    }

    int fd = fileno(rawFp);
    if (isatty(fd)) {
        // best effort -- reconfigure tty for "raw"
        ALOGD("raw video output to tty (fd=%d)", fd);
        struct termios term;
        if (tcgetattr(fd, &term) == 0) {
            cfmakeraw(&term);
            if (tcsetattr(fd, TCSANOW, &term) == 0) {
                ALOGD("tty successfully configured for raw");
            }
        }
    }

    return rawFp;
}
#endif

WLFrameOutput::WLFrameOutput() {
#ifdef OUTPUT_DEBUG
    rawFp = prepareRawOutput("/data/aaa.raw");
#endif
}

void WLFrameOutput::onFrameAvailable(const BufferItem &item) {
    ATRACE_CALL();
    std::ignore = item;
#ifndef GL
    // mWLConsumer->release(item.mSlot, item.mGraphicBuffer);
#endif
    // std::cout << __FUNCTION__ << "\n";
    Mutex::Autolock _l(mMutex);
    mFrameAvailable = true;
    mEventCond.signal();
    // mCv.notify_all();
}

status_t WLFrameOutput::createInputSurface(int width, int height,
                                           sp<IGraphicBufferProducer> *pBufferProducer) {
#ifdef GL
    status_t err;

    err = mEglWindow.createPbuffer(width, height);
    if (err != NO_ERROR) {
        return err;
    }
    mEglWindow.makeCurrent();

    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Shader for rendering the external texture.
    err = mExtTexProgram.setup(Program::PROGRAM_EXTERNAL_TEXTURE);
    if (err != NO_ERROR) {
        return err;
    }

    // Input side (buffers from virtual display).
    glGenTextures(1, &mExtTextureName);
    if (mExtTextureName == 0) {
        ALOGE("glGenTextures failed: %#x", glGetError());
        return UNKNOWN_ERROR;
    }
#endif

    sp<IGraphicBufferProducer> producer;
    sp<IGraphicBufferConsumer> consumer;
    BufferQueue::createBufferQueue(&producer, &consumer);

#ifndef GL
    mWLConsumer = new WLConsumer(consumer, 1, true);
    mWLConsumer->setDefaultBufferSize(width, height);
    mWLConsumer->setFrameAvailableListener(this);
    mWLConsumer->setName(String8("WLFrameOutput"));
#else
    // Input side (buffers from virtual display).
    glGenTextures(1, &mExtTextureName);
    if (mExtTextureName == 0) {
        ALOGE("glGenTextures failed: %#x", glGetError());
        return UNKNOWN_ERROR;
    }
    mGlConsumer = new GLConsumer(consumer, mExtTextureName,
                                 GL_TEXTURE_EXTERNAL_OES, true, false);
    mGlConsumer->setName(String8("virtual display"));
    mGlConsumer->setDefaultBufferSize(width, height);
    mGlConsumer->setConsumerUsageBits(GRALLOC_USAGE_HW_TEXTURE);
    mGlConsumer->setFrameAvailableListener(this);

    mPixelBuf = new uint8_t[width * height * kGlBytesPerPixel];
#endif

    producer->setMaxDequeuedBufferCount(4);
    *pBufferProducer = producer;

    return NO_ERROR;
}

void WLFrameOutput::inputBufferLocked() {
    Mutex::Autolock _l(mMutex);
    if (!mFrameAvailable) {
        nsecs_t timeoutNsec = 250000 * 1000;
        int cc = mEventCond.waitRelative(mMutex, timeoutNsec);
        if (cc == -ETIMEDOUT) {
            return;
        } else if (cc != 0) {
            return;
        }
        return;
    }

    mFrameAvailable = false;

#ifndef GL

    WLConsumer::LockedBuffer imgBuffer;
    // std::cout << __func__ << ":" << __LINE__ << ": before lock next buffer\n";
    auto res = mWLConsumer->lockNextBuffer(&imgBuffer);
    // std::cout << __func__ << ":" << __LINE__ << ": after lock next buffer\n";
    if (res == NOT_ENOUGH_DATA) {
        std::cout << __func__ << ": not enough data\n";
        return;
    } else if (res != OK) {
        std::cout << __func__ << ": not ok  " << strerror(-res) << "\n";
        return;
    }

#ifdef OUTPUT_DEBUG
    int width = imgBuffer.width;
    int height = imgBuffer.height;
    reduceRgbaToRgb(imgBuffer.data, width * height);
    size_t rgbDataLen = width * height * kOutBytesPerPixel;
    fwrite(imgBuffer.data, 1, rgbDataLen, rawFp);

    std::cout << __func__ << ": imgBuffer.width: " << imgBuffer.width << "\n";
    std::cout << __func__ << ": imgBuffer.format: " << imgBuffer.format << "\n";
#endif

    mWLConsumer->unlockBuffer(imgBuffer);

#else
    float texMatrix[16];
    mGlConsumer->updateTexImage();
    mGlConsumer->getTransformMatrix(texMatrix);

    // The data is in an external texture, so we need to render it to the
    // pbuffer to get access to RGB pixel data.  We also want to flip it
    // upside-down for easy conversion to a bitmap.
    int width = mEglWindow.getWidth();
    int height = mEglWindow.getHeight();
    status_t err = mExtTexProgram.blit(mExtTextureName, texMatrix, 0, 0,
                                       width, height, true);
    if (err != NO_ERROR) {
        return;
    }

    GLenum glErr;
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, mPixelBuf);
    if ((glErr = glGetError()) != GL_NO_ERROR) {
        std::cout << "gl error: " << glGetError() << " \n";
        return;
    }

    std::cout << "width: " << width << "  height: " << height << "\n";
#endif
}
