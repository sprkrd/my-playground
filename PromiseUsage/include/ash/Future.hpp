#pragma once

#include <optional>

#include "SharedState.hpp"

namespace ash {
template<class T>
class Promise;

template<class T>
class Future
{
public:
    Future(const Future& other) = delete;
    Future& operator=(const Future& other) = delete;
    Future(Future&& other) noexcept;
    Future& operator=(Future&& other) noexcept;
    bool isValid() const;
    bool isReady() const;
    std::optional<T> tryGet();
    T get();
    ~Future();

private:
    friend Promise<T>;

    void decreaseRefCount();
    explicit Future(impl::SharedState<T>* sharedState);

    impl::SharedState<T>* _sharedState;
};
}

#include "Future.ipp"

