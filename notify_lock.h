#ifndef notifylock_t
#define notifylock_t

#include <atomic>
#include <signal.h>
#include <pthread.h>
#include <mutex>
#include <condition_variable>

class notify_lock{
    private:
        std::mutex mtx{};
        std::condition_variable cv{};
        std::unique_lock<std::mutex> ul{mtx};
        std::atomic<bool> ready{false};
    public:
        notify_lock();
        void lock();
        void unlock();
        void wait();
        void wait_for_ns(int ns);
        void wait_for_ms(int ms);
        void wait_for_s(int s);
};

#endif