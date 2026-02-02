#pragma once
#include <future>

namespace ash {
template<class T>
Future<T>::Future(Future&& other) noexcept : _sharedState(other._sharedState)
{
    other._sharedState = nullptr;
}

template<class T>
Future<T>& Future<T>::operator=(Future&& other) noexcept
{
    if (this != &other)
    {
        if (_sharedState)
        {
            decreaseRefCount();
        }
        _sharedState = other._sharedState;
        other._sharedState = nullptr;
    }
    return *this;
}

template<class T>
bool Future<T>::isValid() const
{
    return static_cast<bool>(_sharedState);
}

template<class T>
bool Future<T>::isReady() const
{
    return isValid() && _sharedState->ready.load(std::memory_order_acquire);
}

template<class T>
std::optional<T> Future<T>::tryGet()
{
    std::optional<T> value;
    if (isReady())
    {
        value = get();
    }
    return value;
}

template<class T>
T Future<T>::get()
{
    if (!_sharedState)
    {
        throw std::future_error(std::future_errc::no_state);
    }
    if (_sharedState->exception)
    {
        auto exception = std::move(_sharedState->exception);
        decreaseRefCount();
        std::rethrow_exception(exception);
    }
    auto value = std::move(*std::launder(reinterpret_cast<T*>(&_sharedState->value)));
    decreaseRefCount();
    return value;
}

template<class T>
Future<T>::~Future()
{
    if (_sharedState)
    {
        decreaseRefCount();
    }
}

template<class T>
void Future<T>::decreaseRefCount()
{
    if (_sharedState->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
    {
        delete _sharedState;
    }
}

template<class T>
Future<T>::Future(impl::SharedState<T>* sharedState) : _sharedState(sharedState)
{
    sharedState->refCount.fetch_add(1, std::memory_order_release);
}
}
