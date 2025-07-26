#include "../include/makemkv_cmd_helper/build_rip_command.h"
#include "../include/makemkv_cmd_helper/exec_rip.h"
#include "../include/makemkv_cmd_helper/loading-spinner.h"
#include <atomic>
#include <future>
#include <iostream>
#include <stdexcept>
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Usage: makemkv_helper destination");
    }

    const std::string destination = argv[1];
    const char *command = "makemkvcon -r info disc:0";

    try {
        std::atomic<bool> finished(false);

        std::future<std::string> rip_command_future = std::async(std::launch::async, buildRipCommand, command, destination, std::ref(finished));
        std::thread loading_spinner_thread(loadingSpinner, std::ref(finished));
        ThreadManager joiner(loading_spinner_thread);

        std::string rip_command = rip_command_future.get();
        execRip(rip_command);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
