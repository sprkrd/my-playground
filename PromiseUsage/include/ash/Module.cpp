#include "Module.hpp"

#include <functional>
#include <iostream>
#include <ostream>
#include <random>
#include <thread>

using namespace ash;

namespace {
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution unif(1000, 2000);
constexpr int BUFFER_SIZE = 1000;
constexpr int PERIOD_MS = 25;

inline std::future<int> getFuture(std::promise<int>& promise)
{
    return promise.get_future();
}

inline Future<int> getFuture(Promise<int>& promise)
{
    return promise.getFuture();
}

inline void setValue(std::promise<int>& promise, int value)
{
    promise.set_value(value);
}

inline void setValue(Promise<int>& promise, int value)
{
    promise.setValue(value);
}
}

Module::Module()
    : _requestBuffer(BUFFER_SIZE)
    , _requestStop(false)
{
    _worker = std::thread(std::bind(&Module::run, this));
}

Module::DelayedResponse Module::postRequest(int x)
{
    Request request;
    request.request = x;
    request.deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(unif(rng));
    auto future = getFuture(request.promise);
    _requestBuffer.push(std::move(request));
    return future;
}

Module::~Module()
{
    std::cout << "Beginning Module::~Module()" << std::endl;
    _requestStop = true;
    _worker.join();
    std::cout << "End Module::~Module()" << std::endl;
}

void Module::run()
{
    while (!_requestStop.load(std::memory_order_relaxed))
    {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(PERIOD_MS);
        tick();
        std::this_thread::sleep_until(deadline);
    }
}

void Module::tick()
{
    auto now = std::chrono::steady_clock::now();
    for (auto requestIt = _activeRequests.begin(); requestIt != _activeRequests.end();)
    {
        auto& request = *requestIt;
        if (now >= request.deadline)
        {
            setValue(request.promise, 2 * request.request);
            requestIt = _activeRequests.erase(requestIt);
        }
        else
        {
            ++requestIt;
        }
    }
    _requestBuffer.popAll(_activeRequests);
}
