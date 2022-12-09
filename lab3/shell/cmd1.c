#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    execl("/bin/ls", "ls", "-l", NULL);
    perror("execl");
    exit(1);
}