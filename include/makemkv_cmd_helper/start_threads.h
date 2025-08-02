#pragma once

#include <string>
#include <thread>

class ThreadManager {
public:
    explicit ThreadManager(std::thread &t) : thread_to_join(t) {}

    ~ThreadManager() {
        if (thread_to_join.joinable()) {
            thread_to_join.join();
        }
    }

private:
    std::thread &thread_to_join;
};

std::string startThreads(const char *command, std::string destination);
