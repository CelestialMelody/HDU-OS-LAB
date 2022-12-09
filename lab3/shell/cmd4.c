#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    char* args[20] = {"grep", "-i", "cmd", "shell.c", NULL, NULL};
    execv("/bin/grep", args);
    perror("execv");
    exit(1);
}