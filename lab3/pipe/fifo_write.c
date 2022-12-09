#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    { // 判断是否输入了文件名
        printf("miss fifoname\n");
        exit(1);
    }
    int ret = access(argv[1], F_OK); // 判断文件是否存在
    if (-1 == ret)
    {
        int ret = mkfifo(argv[1], 0664); // 创建FIFO文件
        if (-1 == ret)
        {
            perror("mkfifo error");
            exit(1);
        }
        else if (0 == ret)
        {
            printf("mkfifo success\n");
        }
    }
    int fd = open(argv[1], O_WRONLY); // 打开FIFO文件 O_RDONLY O_WRONLY O_RDWR

    while (1)
    {
        char buf[1024] = {0};
        if (!scanf("%s", buf))
        {
            close(fd);
            unlink(argv[1]);
            perror("scanf error");
            exit(1);
        }

        write(fd, buf, strlen(buf));

        if (0 == strcmp(buf, "quit") || 0 == strcmp(buf, "exit"))
        {
            close(fd);
            unlink(argv[1]);
            exit(0);
        }
    }

    close(fd);
    return 0;
}