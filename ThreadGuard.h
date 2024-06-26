#pragma once
#ifndef THREAD_GUARD_H
#define THREAD_GUARD_H

#include <vector>
#include <thread>

class ThreadGuard {
    std::vector<std::thread>& threads;

public:
    explicit ThreadGuard(std::vector<std::thread>& threads_)
        : threads(threads_) {}

    // Destructor joins all threads
    ~ThreadGuard() {
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    // Delete copy constructor and copy assignment operator to ensure the guard cannot be copied
    ThreadGuard(const ThreadGuard&) = delete;
    ThreadGuard& operator=(const ThreadGuard&) = delete;
};

#endif // THREAD_GUARD_H
