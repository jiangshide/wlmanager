
#include <binder/IPCThreadState.h>
#include <cutils/atomic.h>
#include <iostream>
#include <sys/syscall.h>

#include "MessageQueue.h"

namespace android {

namespace impl {

void MessageQueue::Handler::dispatchRefresh() {
    if ((android_atomic_or(eventMaskRefresh, &mEventMask) & eventMaskRefresh) == 0) {
        mQueue.mLooper->sendMessage(this, Message(MessageQueue::REFRESH));
    }
}

void MessageQueue::Handler::dispatchInvalidate(nsecs_t expectedVSyncTimestamp) {
    std::ignore = expectedVSyncTimestamp;
//    if ((android_atomic_or(eventMaskInvalidate, &mEventMask) & eventMaskInvalidate) == 0) {
//        mExpectedVSyncTime = expectedVSyncTimestamp;
//        mQueue.mLooper->sendMessage(this, Message(MessageQueue::INVALIDATE));
//    }
}

void MessageQueue::Handler::handleMessage(const Message &message) {
    std::ignore = message;
    int tid = (int)syscall(SYS_gettid);
    int pid = (int)syscall(SYS_getpid);
    std::cout << "handleMessage <<<<<    " << "  tid:  " << tid << "  pid: " << pid << "\n";
//    switch (message.what) {
//        case INVALIDATE:
//            android_atomic_and(~eventMaskInvalidate, &mEventMask);
//            mQueue.mFlinger->onMessageReceived(message.what, mExpectedVSyncTime);
//            break;
//        case REFRESH:
//            android_atomic_and(~eventMaskRefresh, &mEventMask);
//            mQueue.mFlinger->onMessageReceived(message.what, mExpectedVSyncTime);
//            break;
//    }
}

// ---------------------------------------------------------------------------

void MessageQueue::init() {
    mLooper = new Looper(true);
    mHandler = new Handler(*this);
}

#if 0
void MessageQueue::setEventConnection(const sp<EventThreadConnection> &connection) {
    if (mEventTube.getFd() >= 0) {
        mLooper->removeFd(mEventTube.getFd());
    }

    // mEvents = connection;
    // mEvents->stealReceiveChannel(&mEventTube);
    mLooper->addFd(mEventTube.getFd(), 0, Looper::EVENT_INPUT, MessageQueue::cb_eventReceiver,
                   this);
}
#endif

void MessageQueue::waitMessage() {
    do {
        IPCThreadState::self()->flushCommands();
        int32_t ret = mLooper->pollOnce(-1);
        switch (ret) {
            case Looper::POLL_WAKE:
            case Looper::POLL_CALLBACK:
                continue;
            case Looper::POLL_ERROR:
                ALOGE("Looper::POLL_ERROR");
                continue;
            case Looper::POLL_TIMEOUT:
                // timeout (should not happen)
                continue;
            default:
                // should not happen
                ALOGE("Looper::pollOnce() returned unknown status %d", ret);
                continue;
        }
    } while (true);
}

void MessageQueue::postMessage(sp<MessageHandler> &&handler) {
    mLooper->sendMessage(handler, Message());
}

void MessageQueue::invalidate() {
    // mEvents->requestNextVsync();
}

void MessageQueue::refresh() {
    mHandler->dispatchRefresh();
}

#if 0
int MessageQueue::cb_eventReceiver(int fd, int events, void *data) {
    MessageQueue *queue = reinterpret_cast<MessageQueue *>(data);
    return queue->eventReceiver(fd, events);
}
#endif

#if 0
int MessageQueue::eventReceiver(int /*fd*/, int /*events*/) {
    ssize_t n;
    DisplayEventReceiver::Event buffer[8];
    while ((n = DisplayEventReceiver::getEvents(&mEventTube, buffer, 8)) > 0) {
        //        for (int i = 0; i < n; i++) {
        //            if (buffer[i].header.type == DisplayEventReceiver::DISPLAY_EVENT_VSYNC) {
        //                mHandler->dispatchInvalidate(buffer[i].vsync.expectedVSyncTimestamp);
        //                break;
        //            }
        //        }
    }
    return 1;
}
#endif

} // namespace impl
} // namespace android