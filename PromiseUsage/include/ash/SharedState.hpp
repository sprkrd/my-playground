#pragma once

#include <atomic>

namespace ash::impl {
template<class T>
struct alignas(64) SharedState
{
    alignas(alignof(T)) char value[sizeof(T)];
    std::exception_ptr exception;
    std::atomic_bool ready;
    std::atomic_int refCount;

    SharedState();
    ~SharedState();
};
}

#include "SharedState.ipp"

