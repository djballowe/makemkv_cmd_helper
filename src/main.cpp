#include "../include/makemkv_cmd_helper/exec_rip.h"
#include "../include/makemkv_cmd_helper/start_threads.h"
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Usage: makemkv_helper destination");
    }

    const std::string destination = argv[1];
    const char *command = "makemkvcon -r info disc:0";

    try {
        std::string rip_command = startThreads(command, destination);
        execRip(rip_command);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
