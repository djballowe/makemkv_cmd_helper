#include "../include/makemkv_cmd_helper/exec_rip.h"
#include "../include/makemkv_cmd_helper/parse_output.h"
#include <array>
#include <atomic>
#include <exception>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>

namespace {

int selectTitle(std::map<int, TitleSelection> &titles) {
    // std::cout << clear_screen;
    std::cout << "found titles" << std::endl;
    for (const auto &[title, values] : titles) {
        std::cout << "Title: " << title << std::endl;
        std::cout << "------- " << values.name << " | " << values.size << " GB" << std::endl;
        for (const auto &[subKey, subValue] : values.sub_info) {
            std::cout << "------------- " << subValue << std::endl;
        }
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

int currentTitle(std::string line) {
    int curr_title = -1;
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    char split_char = ':';

    while (std::getline(ss, token, split_char)) {
        tokens.push_back(token);
    }

    if (tokens.size()) {
        try {
            curr_title = stoi(tokens[1]);
        } catch (const std::exception &e) {
            throw std::runtime_error("Malformed string could not convert title text to number");
        }
    }

    return curr_title;
}

} // namespace

// add you have to add a struct that will set finished to true no matter what if the function terminates
std::string buildRipCommand(const char *command, const std::string destination, std::atomic<bool> &finished) {
    FILE *fp;
    std::array<char, 256> buffer;
    fp = popen(command, "r");

    if (fp == NULL) {
        throw std::runtime_error("command run failed");
    }

    std::map<int, TitleSelection> titles;
    std::vector<std::string> sub_titles;
    int curr_title = 0;
    int valid_title = -1;
    int curr_subtitle = 0;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        std::string line(buffer.data());

        if (line[0] == 'T') {
            curr_title = currentTitle(line);
            parseTitle(titles, line, valid_title);
        }

        if (line[0] == 'S' && curr_title == valid_title) {
            sub_titles.push_back(line);
        }
    }

    parseSubtitles(titles, sub_titles);

    int close_status = pclose(fp);
    if (close_status == -1) {
        throw std::runtime_error("error closing child process");
    }

    finished = true;
    if (titles.size() == 0) {
        throw std::runtime_error("no titles found");
    }

    int title_selection = selectTitle(titles);

    std::string rip_command = "makemkvcon mkv --progress=-same disc:0 " + std::to_string(title_selection) + " " + destination;

    return rip_command;
}
