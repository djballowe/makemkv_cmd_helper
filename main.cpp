#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

struct RipState {
    std::string current_operation = "";
    std::string current_action = "";
    int current_progress = 0;
    int total_progress = 0;
};

std::vector<std::string> split(std::string split_string, char delimiter) {
    std::string token;
    std::vector<std::string> output;
    std::stringstream ss(split_string);

    while (getline(ss, token, delimiter)) {
        output.push_back(token);
    }

    return output;
}

void parseTitle(std::map<int, std::pair<std::string, int>> &titles, std::string buffer) {
    std::vector<std::string> split_output = split(buffer, '"');
    if (split_output.size() != 3) {
        return;
    }

    std::string title_line = split_output[0];
    std::string name_size = split_output[split_output.size() - 2];

    size_t has_chapters = name_size.find("chapter(s)");
    size_t has_GB = name_size.find("GB");
    if (has_chapters == std::string::npos || has_GB == std::string::npos) {
        return;
    }

    std::vector<std::string> name_size_split = split(name_size, ',');
    std::vector<std::string> title_line_split = split(title_line, ':');
    std::vector<std::string> title = split(title_line_split[1], ',');
    int title_number = stoi(title[0]);
    float_t title_size_GB = stof(name_size_split[1]);
    std::string name = name_size_split[0];

    titles[title_number] = {name, title_size_GB};
    return;
}

std::string buildRipCommand(const char *command) {
    FILE *fp;
    fp = popen(command, "r");

    if (fp == NULL) {
        std::cout << "error" << std::endl;
        throw std::runtime_error("command run failed");
    }

    std::map<int, std::pair<std::string, int>> titles;
    std::array<char, 256> buffer;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        if (buffer[0] == 'T') {
            parseTitle(titles, buffer.data());
        }
    }

    std::cout << "found titles" << std::endl;
    for (const auto pair : titles) {
        std::cout << "Title: " << pair.first << std::endl;
        std::cout << "------- " << pair.second.first << "| " << pair.second.second << " GB" << std::endl;
    }

    int title_selection = -1;
    while (true) {
        std::cout << "select which title you'd like to rip: ";
        std::cin >> title_selection;
        std::cout << std::endl;

        if (std::cin.fail() || titles.find(title_selection) == titles.end()) {
            std::cout << "Error: please select a valid title number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        break;
    }

    std::string rip_command = "makemkvcon mkv --progress=-same disc:0 " + std::to_string(title_selection) + " /mnt/plex_media/test_folder";

    return rip_command;
}

bool parseLoading(std::string line, RipState &state) {
    std::regex progress("Current progress - (\\d+)%  , Total progress - (\\d+)%");
    std::smatch progress_match;
    bool state_change = false;

    std::regex operation("Current operation: (.+)");
    std::smatch operation_match;

    std::regex action("Current action: (.+)");
    std::smatch action_match;

    if (std::regex_search(line, progress_match, progress) && progress_match.size() == 3) {
        state.current_progress = stoi(progress_match[1]);
        state.total_progress = stoi(progress_match[2]);
        state_change = true;
    }

    if (std::regex_search(line, operation_match, operation) && operation_match.size() == 2) {
        state.current_operation = operation_match[1];
        state_change = true;
    }

    if (std::regex_search(line, action_match, action) && action_match.size() == 2) {
        state.current_action = action_match[1];
        state_change = true;
    }

    return state_change;
}

void displayState(RipState state) {
    std::cout << "\033[2J\033[1;1H";
    std::cout << "Current Operation: " << state.current_operation << std::endl;
    std::cout << "Current Action: " << state.current_action << std::endl;
    std::cout << "Current Progress: " << state.current_progress << "%" << std::endl;
    std::cout << "Total Progress: " << state.total_progress << "%" << std::endl;
    return;
}

void execRip(std::string rip_command) {
    FILE *fp;
    fp = popen(rip_command.c_str(), "r");

    if (fp == NULL) {
        std::cout << "error" << std::endl;
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
}

int main() {
    const char *command = "makemkvcon -r info disc:0";
    try {
        std::string rip_command = buildRipCommand(command);
        execRip(rip_command);
    } catch (...) {
        std::cout << "error running command" << std::endl;
        return 1;
    }

    return 0;
}

// TINFO:0,30,0,"Inglourious Basterds - 1
// chapter(s) , 64.3 MB
