#include <iostream>
#include <thread>
#include <ash/Module.hpp>
#include <ash/Promise.hpp>

constexpr int PERIOD_MS = 25;

int main()
{
    std::list<ash::Module::DelayedResponse> responses;
    ash::Module module;
    for (int i = 0; i < 1000; ++i)
    {
        responses.push_back(module.postRequest(i));
    }

    while (!responses.empty())
    {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(PERIOD_MS);
        for (auto it = responses.begin(); it != responses.end();)
        {
            auto& future = *it;
            if (auto result = ash::tryGet(future))
            {
                std::cout << *result << std::endl;
                it = responses.erase(it);
            }
            else
            {
                ++it;
            }
        }
        std::this_thread::sleep_until(deadline);
    }
}
