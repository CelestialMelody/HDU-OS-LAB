#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    char cmd[100];
    char *cmds[20] = {NULL};
    int child_proc_status;
    pid_t pid;

    while (1)
    {
        printf("shell:");
        fgets(cmd, 100, stdin);
        char cmd_copy[100];
        strcpy(cmd_copy, cmd);

        int cnt = 0;
        // split the command into tokens
        // strtok 用法：https://www.runoob.com/cprogramming/c-function-strtok.html
        cmds[cnt] = strtok(cmd_copy, " \n");
        while (cmds[cnt])
        {
            cmds[++cnt] = strtok(NULL, " \n");
        }
        cmds[cnt] = NULL;

        child_proc_status = -1;
        if (!strcmp(cmds[0], "cmd1") || !strcmp(cmds[0], "cmd2") || !strcmp(cmds[0], "cmd3"))
        {
            // fork 子进程中返回 0 值, 而父进程中返回子进程ID
            pid = fork();
            if (pid < 0)
            {
                printf("fork error\n");
            }
            else if (pid > 0)
            {                             // parent proc
                wait(&child_proc_status); // wait child proc end
            }
            else
            {
                char cmdf[102];
                sprintf(cmdf, "./%s", cmds[0]);
                system(cmdf);
                exit(0);
            }
        }
        else if (!strcmp(cmds[0], "exit"))
        {
            printf("shell exit\n");
            break;
        }
        else if (!strcmp(cmds[0], "find"))
        {
            system(cmd);
        }
        else if (!strcmp(cmds[0], "grep"))
        {
            system(cmd);
        }
        else if (!strcmp(cmds[0], "clear"))
        {
            system(cmd);
        }
        else
        {
            printf("Command not found\n");
        }
    }
}