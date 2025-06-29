#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <stdio.h>
#include <string>

struct RipState {
    std::string current_operation = "";
    std::string current_action = "";
    int current_progress = 0;
    int total_progress = 0;
};

struct TitleSelection {
    std::string name = "";
    int title_number = -1;
    double size = -1.0;
};

void parseTitle(std::vector<TitleSelection> &titles, std::string &line) {
    static std::regex reg(R"(TINFO:([0-9]+),[0-9]+,[0-9]+,\"((?:.*)?[0-9]+ chapter\(s\)) , ([0-9]*(?:[.][0-9]+)?) GB\")");
    std::smatch title_match;
    TitleSelection current_title;

    if (std::regex_search(line, title_match, reg) && title_match.size() == 4) {
        int title_number = stoi(title_match[1]);
        std::string title_name = title_match[2];
        double title_size_gb = stof(title_match[3]);

        current_title.name = title_name;
        current_title.title_number = title_number;
        current_title.size = title_size_gb;
        titles.push_back(current_title);
        return;
    }

    return;
}

int selectTitle(std::vector<TitleSelection> &titles) {
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

std::string buildRipCommand(const char *command, const std::string destination) {
    FILE *fp;
    fp = popen(command, "r");

    if (fp == NULL) {
        std::cout << "error" << std::endl;
        throw std::runtime_error("command run failed");
    }

    std::vector<TitleSelection> titles;
    std::array<char, 256> buffer;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        std::string line(buffer.data());
        if (line[0] == 'T') {
            std::cout << line << std::endl;
            parseTitle(titles, line);
        }
    }

    int close_status = pclose(fp);
    if (close_status == -1) {
        throw std::runtime_error("error closing child process");
    }

    if (titles.size() == 0) {
        throw std::runtime_error("no titles found");
    }

    int title_selection = selectTitle(titles);

    std::string rip_command = "makemkvcon mkv --progress=-same disc:0 " + std::to_string(titles[title_selection].title_number) + " " + destination;

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

    int close_status = pclose(fp);
    if (close_status == -1) {
        throw std::runtime_error("error closing child process");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        throw std::runtime_error("Usage: makemkv_helper destination");
    }

    const std::string destination = argv[1];
    const char *command = "makemkvcon -r info disc:0";

    try {
        std::string rip_command = buildRipCommand(command, destination);
        execRip(rip_command);
    } catch (std::runtime_error &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}
