#pragma once
#include "Future.hpp"
#include "SharedState.hpp"

namespace ash {
template<class T>
class Promise
{
public:
    Promise();
    Promise(const Promise& other) = delete;
    Promise& operator=(const Promise& other) = delete;
    Promise(Promise&& other) noexcept;
    Promise& operator=(Promise&& other) noexcept;
    Future<T> getFuture();
    void setException(std::exception_ptr e);
    void setValue(T value);
    ~Promise();

private:
    void decreaseRefCount();

    impl::SharedState<T>* _sharedState;
    bool _futureRetrieved;
    bool _valueSet;
};
}

#include "Promise.ipp"
