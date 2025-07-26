#include "../include/makemkv_cmd_helper/parse_output.h"
#include <array>
#include <iostream>

std::string clear_screen = "\033[2J\033[1;1H";

namespace {

void displayState(RipState state) {
    std::cout << clear_screen;
    std::cout << "Current Operation: " << state.current_operation << std::endl;
    std::cout << "Current Action: " << state.current_action << std::endl;
    std::cout << "Current Progress: " << state.current_progress << "%" << std::endl;
    std::cout << "Total Progress: " << state.total_progress << "%" << std::endl;
    return;
}

} // namespace

void execRip(std::string rip_command) {
    FILE *fp;
    fp = popen(rip_command.c_str(), "r");

    if (fp == NULL) {
        throw std::runtime_error("command run failed");
    }

    std::array<char, 256> buffer;

    RipState state;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        std::string line(buffer.data());
        bool state_change = parseLoading(line, state);

        if (state_change) {
            displayState(state);
        }
    }

    int close_status = pclose(fp);
    if (close_status == -1) {
        throw std::runtime_error("error closing child process");
    }
}
