#include <stdio.h>

int main() {
    const char *command = "makemkvcon -r info disc:0 | grep GB";
    FILE *fp;
    fp = popen(command, "r");

    if (fp == NULL) {
        printf("failed to run command: %s", command);
        return -1;
    }

    char buffer[40];

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s\n", buffer);
    }

    return 0;
}
