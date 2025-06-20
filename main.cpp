#include <iostream>
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

int main() {
    const char *command = "makemkvcon -r info disc:0";
    FILE *fp;
    fp = popen(command, "r");

    vector<string> output;

    if (fp == NULL) {
        printf("failed to run command: %s", command);
        return -1;
    }

    char buffer[40];

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        output.push_back(buffer);
    }

    for (string val : output) {
        cout << val << endl;
    }

    return 0;
}
