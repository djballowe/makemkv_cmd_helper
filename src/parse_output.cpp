#include "../include/makemkv_cmd_helper/parse_output.h"
#include <regex>
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

void parseTitle(std::vector<TitleSelection> &titles, std::string &line) {
    static std::regex reg(R"(TINFO:([0-9]+),[0-9]+,[0-9]+,\"((?:.*)?[0-9]+ chapter\(s\)) , ([0-9]*(?:[.][0-9]+)?) GB\")");
    std::smatch title_match;
    TitleSelection current_title;

    if (std::regex_search(line, title_match, reg) && title_match.size() == 4) {
        int title_number = stoi(title_match[1]);
        std::string title_name = title_match[2];
        double title_size_gb = std::stod(title_match[3]);

        current_title.name = title_name;
        current_title.title_number = title_number;
        current_title.size = title_size_gb;
        titles.push_back(current_title);
        return;
    }

    return;
}
