#include "../include/makemkv_cmd_helper/parse_output.h"
#include "../include/makemkv_cmd_helper/exec_rip.h"
#include <array>
#include <atomic>
#include <iostream>
#include <limits>

namespace {

int selectTitle(std::vector<TitleSelection> &titles) {
    std::cout << clear_screen;
    std::cout << "found titles" << std::endl;
    for (int i = 0; i < titles.size(); i++) {
        std::cout << "Title: " << i << std::endl;
        std::cout << "------- " << titles[i].name << "| " << titles[i].size << " GB" << std::endl;
    }

    int title_selection = -1;
    while (true) {
        std::cout << "select which title you'd like to rip: ";
        std::cin >> title_selection;
        std::cout << std::endl;

        if (std::cin.fail() || title_selection >= titles.size()) {
            std::cout << "Error: please select a valid title number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        break;
    }

    return title_selection;
}

} // namespace

// add you have to add a sturct that will set finished to true no matter what if the function terminates
std::string buildRipCommand(const char *command, const std::string destination, std::atomic<bool> &finished) {
    FILE *fp;
    fp = popen(command, "r");

    if (fp == NULL) {
        throw std::runtime_error("command run failed");
    }

    std::vector<TitleSelection> titles;
    std::array<char, 256> buffer;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        std::string line(buffer.data());
        if (line[0] == 'T') {
            parseTitle(titles, line);
        }
    }

    int close_status = pclose(fp);
    if (close_status == -1) {
        throw std::runtime_error("error closing child process");
    }

    finished = true;
    if (titles.size() == 0) {
        throw std::runtime_error("no titles found");
    }

    int title_selection = selectTitle(titles);

    std::string rip_command = "makemkvcon mkv --progress=-same disc:0 " + std::to_string(titles[title_selection].title_number) + " " + destination;

    return rip_command;
}
