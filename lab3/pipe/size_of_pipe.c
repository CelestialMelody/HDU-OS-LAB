#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

// C语言中多线程之间共享全局变量data段数据
int i = 0;

void *continue_write()
{
    int fd[2] = {0};

    int ret = pipe(fd);
    if (ret == -1)
    {
        perror("pipe");
        exit(1);
    }
    for (i = 0; i < (int)1e6; i++)
    {                         // 阻塞时， i不再增加
        write(fd[1], "c", 1); // write 1 byte to pipe
    }
    close(fd[0]); // close read end
    close(fd[1]); // close write end
}

int main()
{
    pthread_t tid;
    int lastnum = -1;
    pthread_create(&tid, NULL, continue_write, NULL); // create a thread to write to pipe
    while (1)
    {
        if (i != lastnum)
        { // 未阻塞
            lastnum = i;
            usleep(1000);
        }
        else
        { // 阻塞时，i == lastnum
            printf("pipe size is %d bytes\n", lastnum);
            break;
        }
    }
    return 0;
}