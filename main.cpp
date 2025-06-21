#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

std::string testString = R"(TINFO:2,10,0,"61.2 GB"
TINFO:2,30,0,"Inglourious Basterds - 28 chapter(s) , 61.2 GB"
TINFO:3,10,0,"10.6 GB"
TINFO:3,30,0,"Inglourious Basterds - 7 chapter(s) , 10.6 GB"
TINFO:4,10,0,"4.8 GB"
TINFO:4,30,0,"Inglourious Basterds - 7 chapter(s) , 4.8 GB"
TINFO:7,10,0,"1.3 GB"
TINFO:7,30,0,"Inglourious Basterds - 3 chapter(s) , 1.3 GB"
TINFO:12,10,0,"1.7 GB"
TINFO:12,30,0,"Inglourious Basterds - 3 chapter(s) , 1.7 GB"
TINFO:17,10,0,"1.0 GB"
TINFO:17,30,0,"Inglourious Basterds , 1.0 GB"
TINFO:18,10,0,"1.1 GB"
TINFO:18,30,0,"Inglourious Basterds , 1.1 GB")";

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
    std::vector<std::string> output;
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
        std::cout << "------- " << pair.second.first << " | " << pair.second.second << " GB" << std::endl;
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

    std::string rip_command = "makemkvcon -r --progress=-same disc:0 " + std::to_string(title_selection) + " /mnt/plex_media/movies";

    return rip_command;
}

std::string execRip(std::string rip_command) {
    
    return "";
}

int main() {
    const char *command = "makemkvcon -r info disc:0";
    try {
        std::string rip_command = buildRipCommand(command);
        std::string output = execRip(rip_command);
    } catch (...) {
        std::cout << "error running command" << std::endl;
        return 1;
    }

    return 0;
}

// TINFO:0,30,0,"Inglourious Basterds - 1
// chapter(s) , 64.3 MB
