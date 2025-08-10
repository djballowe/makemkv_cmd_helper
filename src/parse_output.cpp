#include "../include/makemkv_cmd_helper/parse_output.h"
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

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

void parseTitle(std::map<int, TitleSelection> &titles, std::string line, int &valid_title) {
    static std::regex reg(R"(TINFO:([0-9]+),[0-9]+,[0-9]+,\"((?:.*)?[0-9]+ chapter\(s\)) , ([0-9]*(?:[.][0-9]+)?) GB\")");
    std::smatch title_match;
    TitleSelection current_title;

    if (std::regex_search(line, title_match, reg) && title_match.size() == 4) {
        int title_number = stoi(title_match[1]);
        std::string title_name = title_match[2];
        double title_size_gb = std::stod(title_match[3]);

        valid_title = title_number;

        current_title.name = title_name;
        current_title.size = title_size_gb;
        titles[title_number] = current_title;
        return;
    }

    return;
}

void parseSubtitles(std::map<int, TitleSelection> &titles, std::vector<std::string> &sub_titles) {
    std::map<int, std::string> sorted_sub_titles;

    for (std::string sub_title : sub_titles) {
        static std::regex reg(R"(SINFO:([0-9]+),([0-9]+),([0-9]+),([0-9]+),\"(.*)?\")");
        std::smatch subtitle_match;

        int prev_sub_title = 0;

        if (std::regex_search(sub_title, subtitle_match, reg) && subtitle_match.size() == 6) {
            int title = stoi(subtitle_match[1]);
            int sub_title_group = stoi(subtitle_match[2]);
            std::string sub_title_data = subtitle_match[5];

            if (titles.find(title) != titles.end()) {
                titles[title].sub_info[sub_title_group] += (" " + sub_title_data + ",");
            } else {
                throw std::runtime_error("title string mismatch");
            }
        }
    }
}
