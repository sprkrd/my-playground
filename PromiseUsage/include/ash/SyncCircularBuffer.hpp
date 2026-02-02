#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <boost/circular_buffer.hpp>

namespace ash {
template<class T>
class SyncCircularBuffer
{
public:
    explicit SyncCircularBuffer(size_t size);

    template<class U>
    bool push(U&& value, std::chrono::milliseconds timeoutMs);

    template<class U>
    bool push(U&& value);

    template<class U>
    bool tryPush(U&& value);

    std::optional<T> pop(std::chrono::milliseconds timeoutMs);
    std::optional<T> pop();
    std::optional<T> tryPop();

    template<class Collection>
    void popAll(Collection& values);

private:
    T doPop();

    static constexpr auto MAX_WAIT_MS = std::chrono::milliseconds::max();
    mutable std::mutex _mutex;
    std::condition_variable _cvFull;
    std::condition_variable _cvEmpty;
    boost::circular_buffer<T> _buffer;
};
}

#include "SyncCircularBuffer.ipp"

