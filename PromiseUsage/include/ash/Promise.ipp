#pragma once
#include <future>

namespace ash {
template<class T>
Promise<T>::Promise() : _sharedState(new impl::SharedState<T>)
                      , _futureRetrieved(false)
                      , _valueSet(false)
{}

template<class T>
Promise<T>::Promise(Promise&& other) noexcept
    : _sharedState(other._sharedState)
    , _futureRetrieved(other._futureRetrieved)
    , _valueSet(other._valueSet)
{
    other._sharedState = nullptr;
}

template<class T>
Promise<T>& Promise<T>::operator=(Promise&& other) noexcept
{
    if (this != &other)
    {
        if (_sharedState)
        {
            decreaseRefCount();
        }
        _sharedState = other._sharedState;
        _futureRetrieved = other._futureRetrieved;
        _valueSet = other._valueSet;
        other._sharedState = nullptr;
    }
    return *this;
}

template<class T>
Future<T> Promise<T>::getFuture()
{
    if (_futureRetrieved)
    {
        throw std::future_error(std::future_errc::future_already_retrieved);
    }
    _futureRetrieved = true;
    return Future<T>(_sharedState);
}

template<class T>
void Promise<T>::setException(std::exception_ptr e)
{
    if (!_sharedState)
    {
        throw std::future_error(std::future_errc::no_state);
    }
    if (_valueSet)
    {
        throw std::future_error(std::future_errc::promise_already_satisfied);
    }
    _valueSet = true;
    _sharedState->exception = std::move(e);
    _sharedState->ready.store(true, std::memory_order_release);
}

template<class T>
void Promise<T>::setValue(T value)
{
    if (!_sharedState)
    {
        throw std::future_error(std::future_errc::no_state);
    }
    if (_valueSet)
    {
        throw std::future_error(std::future_errc::promise_already_satisfied);
    }
    _valueSet = true;
    new(&_sharedState->value) T(std::move(value));
    _sharedState->ready.store(true, std::memory_order_release);
}

template<class T>
Promise<T>::~Promise()
{
    if (_sharedState)
    {
        if (!_valueSet)
        {
            _sharedState->exception = std::make_exception_ptr(std::future_error(std::future_errc::broken_promise));
            _sharedState->ready.store(true, std::memory_order_release);
        }
        decreaseRefCount();
    }
}

template<class T>
void Promise<T>::decreaseRefCount()
{
    if (_sharedState->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
    {
        delete _sharedState;
    }
}
}
