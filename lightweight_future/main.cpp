#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <thread>
#include <type_traits>

#include <boost/circular_buffer.hpp>


template<class T>
class SyncCircularBuffer
{
public:
    SyncCircularBuffer(size_t size)
        : _buffer(size)
    {}
    
    template<class U>
    bool push(U&& value, unsigned int timeoutMs)
    {
        static_assert(std::is_constructible_v<T,U&&>, "Can't construct: incompatible types or non-copyable class");
        std::unique_lock lock(_mutex);
        auto deadline = timeoutMs == MAX_WAIT_MS?
                std::chrono::steady_clock::time_point::max() :
                std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        bool onTime = _cvFull.wait_until(lock, deadline,
                [this](){ return !_buffer.full(); });
        if (onTime)
        {
            _buffer.push_back(std::forward<U>(value));
            lock.unlock();
            _cvEmpty.notify_one();
            return true;
        }
        return false;
    }
    
    template<class U>
    bool push(U&& value)
    {
        return push(std::forward<U>(value), MAX_WAIT_MS);
    }
    
    template<class U>
    bool tryPush(U&& value)
    {
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
    
    std::optional<T> pop(unsigned int timeoutMs)
    {
        std::unique_lock lock(_mutex);
        auto deadline = timeoutMs == MAX_WAIT_MS?
                std::chrono::steady_clock::time_point::max() :
                std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        bool onTime = _cvEmpty.wait_until(lock, deadline,
            [this](){ return !_buffer.empty(); });
        std::optional<T> value;
        if (onTime)
        {
            value = std::move(_buffer.front());
            _buffer.pop_front();
            lock.unlock();
            _cvFull.notify_one();
        }
        return value;
    }
    
    std::optional<T> pop()
    {
        return pop(MAX_WAIT_MS);
    }
    
    std::optional<T> tryPop()
    {
        std::optional<T> value;
        if (auto lock = std::unique_lock(_mutex, std::try_to_lock))
        {
            if (!_buffer.empty())
            {
                value = std::move(_buffer.front());
                _buffer.pop_front();
                lock.unlock();
                _cvFull.notify_one();
            }
        }
        return value;
    }

private:
    static constexpr auto MAX_WAIT_MS = std::numeric_limits<unsigned int>::max();
    mutable std::mutex _mutex;
    std::condition_variable _cvFull;
    std::condition_variable _cvEmpty;
    boost::circular_buffer<T> _buffer;
};

template<class T>
struct alignas(64) SharedState
{
    std::optional<T> value = {};
    std::atomic_bool ready = false;
};

template<class T>
using SharedStatePtr = std::shared_ptr<SharedState<T>>; 

template<class T>
class Promise;

template<class T>
class DelayedResult
{
    public:
        DelayedResult(const DelayedResult& other) = delete;
        DelayedResult& operator=(const DelayedResult& other) = delete;
        
        DelayedResult(DelayedResult&& other) = default;
        DelayedResult& operator=(DelayedResult&& other) = default;
    
        bool isValid() const
        {
            return static_cast<bool>(_sharedState);
        }
        
        bool isReady() const
        {
            return isValid() && _sharedState->ready.load(std::memory_order_acquire);
        }
    
        std::optional<T> tryGet()
        {
            std::optional<T> value;
            if (isReady())
            {
                value = get();
            }
            return value;
        }
        
        T get()
        {
            auto value = std::move(_sharedState->value);
            _sharedState.reset();
            return *value;
        }
    
    private:
        friend Promise<T>;
        
        DelayedResult(SharedStatePtr<T> sharedState) : _sharedState(std::move(sharedState))
        {}
        
        SharedStatePtr<T> _sharedState;
};

template<class T>
class Promise
{
    public:
        Promise()
            : _sharedState(std::make_shared<SharedState<T>>())
            , _resultRetrieved(false)
            , _valueSet(false)
        {}
        
        Promise(const Promise& other) = delete;
        Promise& operator=(const Promise& other) = delete;
        
        Promise(Promise&& other) = default;
        Promise& operator=(Promise&& other) = default;
        
        DelayedResult<T> getResult()
        {
            if (_resultRetrieved)
            {
                throw std::future_error(std::future_errc::future_already_retrieved);
            }
            _resultRetrieved = true;
            return DelayedResult<T>(_sharedState);
        }
        
        void setValue(T value)
        {
            if (_valueSet)
            {
                throw std::future_error(std::future_errc::promise_already_satisfied);
            }
            _valueSet = true;
            _sharedState->value = std::move(value);
            _sharedState->ready.store(true, std::memory_order_release);
        }
    
    private:
        SharedStatePtr<T> _sharedState;
        bool _resultRetrieved;
        bool _valueSet;
};

uint64_t fib(uint64_t n)
{
    if (n <= 1) return 1;
    return fib(n-1) + fib(n-2);
}

int main()
{
    Promise<uint64_t> promise;
    // auto result = promise.getResult();
    
    // std::thread t([p=std::move(promise)]() mutable { p.setValue(fib(42)); });
    
    // while (!result.isReady())
    // {
        // std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // std::cout << "Waiting..." << std::endl;
    // }
    // std::cout << result.get() << std::endl;
    
    // t.join();
    
    SyncCircularBuffer<std::string> buffer(100);
    
    std::thread t1([&]()
    {
        for (int i = 0; i < 100; ++i)
        {
            buffer.push("abc");
        }
    });
    
    std::thread t2([&]()
    {
        for (int i = 0; i < 100; ++i)
        {
            if (auto x = buffer.pop())
            {
                std::cout << *x << std::endl;
            }
        }
    });
    
    t1.join();
    t2.join();
    
    // auto f = promise.getResult();
    
    // SyncCircularBuffer<Promise<uint64_t>> buffer(100);
    // buffer.push(std::move(promise));
    
    // while (auto p = buffer.tryPop())
    // {
        // p->setValue(42);
    // }
    
    // if (auto value = f.tryGet())
    // {
        // std::cout << "value " << *value << std::endl;
    // }
    // else
    // {
        // std::cout << "no value" << std::endl;
    // }
}
