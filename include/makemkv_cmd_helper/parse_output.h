#pragma once

#include <string>
#include <vector>

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
    std::vector<std::string> sub_info;
};

bool parseLoading(std::string line, RipState &state);

void parseTitle(std::vector<TitleSelection> &titles, std::string line, int &valid_title);

void addSubtitle(std::vector<TitleSelection> &titles, std::string line);
