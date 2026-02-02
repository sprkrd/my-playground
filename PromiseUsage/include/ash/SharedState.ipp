#pragma once

namespace ash::impl {
template<class T>
SharedState<T>::SharedState() : value{0}
                              , ready(false)
                              , refCount(1)
{}

template<class T>
SharedState<T>::~SharedState()
{
    if (ready.load(std::memory_order_acquire) && !exception)
    {
        std::launder(reinterpret_cast<T*>(&value))->~T();
    }
}
}

