#pragma once

namespace ash {
template<class T>
SyncCircularBuffer<T>::SyncCircularBuffer(size_t size) : _buffer(size)
{}

template<class T>
template<class U>
bool SyncCircularBuffer<T>::push(U&& value, std::chrono::milliseconds timeoutMs)
{
    static_assert(std::is_constructible_v<T, U&&>, "Can't construct: incompatible types or non-copyable class");
    std::unique_lock lock(_mutex);
    auto deadline = timeoutMs == MAX_WAIT_MS ?
                        std::chrono::steady_clock::time_point::max() :
                        std::chrono::steady_clock::now() + timeoutMs;
    if (_cvFull.wait_until(lock, deadline, [this]() { return !_buffer.full(); }))
    {
        _buffer.push_back(std::forward<U>(value));
        lock.unlock();
        _cvEmpty.notify_one();
        return true;
    }
    return false;
}

template<class T>
template<class U>
bool SyncCircularBuffer<T>::push(U&& value)
{
    return push(std::forward<U>(value), MAX_WAIT_MS);
}

template<class T>
template<class U>
bool SyncCircularBuffer<T>::tryPush(U&& value)
{
    static_assert(std::is_constructible_v<T, U&&>, "Can't construct: incompatible types or non-copyable class");
    if (auto lock = std::unique_lock(_mutex, std::try_to_lock))
    {
        if (!_buffer.full())
        {
            _buffer.push_back(std::forward<U>(value));
            lock.unlock();
            _cvEmpty.notify_one();
            return true;
        }
    }
    return false;
}

template<class T>
std::optional<T> SyncCircularBuffer<T>::pop(std::chrono::milliseconds timeoutMs)
{
    std::unique_lock lock(_mutex);
    auto deadline = timeoutMs == MAX_WAIT_MS ?
                        std::chrono::steady_clock::time_point::max() :
                        std::chrono::steady_clock::now() + timeoutMs;
    std::optional<T> value;
    if (_cvEmpty.wait_until(lock, deadline, [this]() { return !_buffer.empty(); }))
    {
        value = doPop();
        lock.unlock();
        _cvFull.notify_one();
    }
    return value;
}

template<class T>
std::optional<T> SyncCircularBuffer<T>::pop()
{
    return pop(MAX_WAIT_MS);
}

template<class T>
std::optional<T> SyncCircularBuffer<T>::tryPop()
{
    std::optional<T> value;
    if (auto lock = std::unique_lock(_mutex, std::try_to_lock))
    {
        if (!_buffer.empty())
        {
            value = doPop();
            lock.unlock();
            _cvFull.notify_one();
        }
    }
    return value;
}

template<class T>
template<class Collection>
void SyncCircularBuffer<T>::popAll(Collection& values)
{
    std::unique_lock lock(_mutex);
    while (!_buffer.empty())
    {
        values.push_back(doPop());
    }
    lock.unlock();
    _cvFull.notify_one();
}

template<class T>
T SyncCircularBuffer<T>::doPop()
{
    auto value = std::move(_buffer.front());
    _buffer.pop_front();
    return value;
}
}
