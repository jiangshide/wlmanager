
#ifndef WLMANAGER_SRC_MESSAGEQUEUE_H_
#define WLMANAGER_SRC_MESSAGEQUEUE_H_

#include <cstdint>
#include <future>
#include <type_traits>
#include <utility>

#include <utils/Looper.h>
#include <utils/Timers.h>
#include <utils/threads.h>

namespace android {

template<typename F>
class Task : public MessageHandler {
    template<typename G>
    friend auto makeTask(G &&);

    explicit Task(F &&f) : mTask(std::move(f)) {}

    void handleMessage(const Message &) override { mTask(); }

    using T = std::invoke_result_t<F>;
    std::packaged_task<T()> mTask;
};

template<typename F>
inline auto makeTask(F &&f) {
    sp<Task<F>> task = new Task<F>(std::move(f));
    return std::make_pair(task, task->mTask.get_future());
}

class MessageQueue {
 public:
    enum {
        INVALIDATE = 0,
        REFRESH = 1,
    };

    virtual ~MessageQueue() = default;

    virtual void init() = 0;
    // virtual void setEventConnection(const sp<EventThreadConnection>& connection) = 0;
    virtual void waitMessage() = 0;
    virtual void postMessage(sp<MessageHandler> &&) = 0;
    virtual void invalidate() = 0;
    virtual void refresh() = 0;
};

// ---------------------------------------------------------------------------

namespace impl {

class MessageQueue final : public android::MessageQueue {
    class Handler : public MessageHandler {
        enum { eventMaskInvalidate = 0x1,
               eventMaskRefresh = 0x2,
               eventMaskTransaction = 0x4 };
        MessageQueue &mQueue;
        int32_t mEventMask;
        // std::atomic<nsecs_t> mExpectedVSyncTime;

     public:
        explicit Handler(MessageQueue &queue) : mQueue(queue), mEventMask(0) {
        }
        virtual void handleMessage(const Message &message);
        void dispatchRefresh();
        void dispatchInvalidate(nsecs_t expectedVSyncTimestamp);
    };

    friend class Handler;

    sp<Looper> mLooper;
    // sp<EventThreadConnection> mEvents;
    // gui::BitTube mEventTube;
    sp<Handler> mHandler;

    // static int cb_eventReceiver(int fd, int events, void *data);
    // int eventReceiver(int fd, int events);

 public:
    ~MessageQueue() override = default;
    void init() override;
    // void setEventConnection(const sp<EventThreadConnection>& connection) override;

    void waitMessage() override;
    void postMessage(sp<MessageHandler> &&) override;

    // sends INVALIDATE message at next VSYNC
    void invalidate() override;

    // sends REFRESH message at next VSYNC
    void refresh() override;
};

}// namespace impl
} // namespace android
#endif