#include "../include/makemkv_cmd_helper/build_rip_command.h"
#include "../include/makemkv_cmd_helper/loading-spinner.h"
#include "../include/makemkv_cmd_helper/start_threads.h"
#include <atomic>
#include <future>
#include <string>

std::string startThreads(const char *command, std::string destination) {
    std::atomic<bool> finished(false);
    std::future<std::string> rip_command_future = std::async(std::launch::async, buildRipCommand, command, destination, std::ref(finished));
    std::thread loading_spinner_thread(loadingSpinner, std::ref(finished));
    ThreadManager joiner(loading_spinner_thread);

    return rip_command_future.get();
}
