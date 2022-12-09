#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    execl("/bin/pwd", "pwd", NULL);
    perror("execl");
    exit(1);
}