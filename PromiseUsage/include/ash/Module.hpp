#pragma once

// #define USE_STD_FUTURE

#include <list>

#include "Promise.hpp"
#include "SyncCircularBuffer.hpp"

namespace ash {
inline std::optional<int> tryGet(std::future<int>& future)
{
    if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        return future.get();
    }
    return {};
}

inline std::optional<int> tryGet(Future<int>& future)
{
    return future.tryGet();
}

class Module
{
public:
#ifdef USE_STD_FUTURE
    using DelayedResponse = std::future<int>;
#else
    using DelayedResponse = Future<int>;
#endif

    Module();
    DelayedResponse postRequest(int x);
    ~Module();

private:
    struct Request
    {
#ifdef USE_STD_FUTURE
        std::promise<int> promise;
#else
        Promise<int> promise;
#endif

        int request;
        std::chrono::steady_clock::time_point deadline;
    };

    void run();
    void tick();

    std::thread _worker;
    SyncCircularBuffer<Request> _requestBuffer;
    std::list<Request> _activeRequests;
    std::atomic_bool _requestStop;
};
}

