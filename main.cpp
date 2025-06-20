#include <array>
#include <cstddef>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

string testString = R"(TINFO:2,10,0,"61.2 GB"
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

vector<string> split(string split_string, char delimiter) {
    string token;
    vector<string> output;
    stringstream ss(split_string);

    while (getline(ss, token, delimiter)) {
        output.push_back(token);
    }

    return output;
}

void parseTitle(map<int, pair<string, int>> &titles, string buffer) {
    vector<string> split_output = split(buffer, '"');
    if (split_output.size() != 3) {
        return;
    }

    string title_line = split_output[0];
    string name_size = split_output[split_output.size() - 2];

    size_t has_chapters = name_size.find("chapter(s)");
    size_t has_GB = name_size.find("GB");
    if (has_chapters == string::npos || has_GB == string::npos) {
        return;
    }

    vector<string> name_size_split = split(name_size, ',');
    vector<string> title_line_split = split(title_line, ':');
    vector<string> title = split(title_line_split[1], ',');
    int title_number = stoi(title[0]);

    cout << title_number << endl;

    return;
}

vector<string> execMake(const char *command) {
    vector<string> output;
    FILE *fp;
    fp = popen(command, "r");

    if (fp == NULL) {
        cout << "error" << endl;
        throw runtime_error("command run failed");
    }

    map<int, pair<string, int>> titles;
    array<char, 256> buffer;

    while (fgets(buffer.data(), sizeof(buffer), fp) != NULL) {
        if (buffer[0] == 'T') {
            parseTitle(titles, buffer.data());
            // cout << buffer.data() << endl;
            // output.push_back(buffer.data());
        }
    }

    // for (string val : output) {
    //     cout << val << endl;
    // }

    return output;
}

int main() {
    const char *command = "makemkvcon -r info disc:0";
    try {
        vector<string> outputString = execMake(command);
    } catch (...) {
        cout << "error running command" << endl;
        return 1;
    }

    return 0;
}

// TINFO:0,30,0,"Inglourious Basterds - 1
// chapter(s) , 64.3 MB
