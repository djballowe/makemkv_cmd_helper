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

void parseTitle(std::map<int, std::pair<std::string, int>> &titles,
                std::string buffer) {
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
    std::string size = name_size_split[1];

    float_t title_size_GB = stof(size);

    std::cout << title_size_GB << std::endl;

    return;
}

std::vector<std::string> execMake(const char *command) {
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
            // cout << buffer.data() << endl;
            // output.push_back(buffer.data());
        }
    }

    // for (std::string val : output) {
    //     cout << val << endl;
    // }

    return output;
}

int main() {
    const char *command = "makemkvcon -r info disc:0";
    try {
        std::vector<std::string> outputString = execMake(command);
    } catch (...) {
        std::cout << "error running command" << std::endl;
        return 1;
    }

    return 0;
}

// TINFO:0,30,0,"Inglourious Basterds - 1
// chapter(s) , 64.3 MB
