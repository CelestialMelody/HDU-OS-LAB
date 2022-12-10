#include "init.h"

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

    // 借助信号量
    init(); // 可以不借助信号量

    int fd = open(argv[1], O_WRONLY); // 打开FIFO文件 O_RDONLY O_WRONLY O_RDWR

    while (1)
    {
        char buf[1024] = {0};
        // if (!scanf("%s", buf)) // scanf 忽略空格 换行
        if (!fgets(buf, sizeof(buf), stdin)) // fgets 读取换行符
        {
            close(fd);
            unlink(argv[1]);
            perror("scanf error");
            exit(1);
        }

        sem_wait(sem_send);
        sem_wait(mutex);
        write(fd, buf, strlen(buf) - 1); // -1 去掉换行符
        sem_post(mutex);
        sem_post(sem_receive);

        // 可以不借助信号量
        // write(fd, buf, strlen(buf) - 1);

        // if (0 == strcmp(buf, "quit") || 0 == strcmp(buf, "exit")) // 使用 scanf
        if (0 == strncmp(buf, "quit", 4) || 0 == strncmp(buf, "exit", 4)) // 使用 fgets
        {
            close(fd);
            unlink(argv[1]);
            exit(0);
        }
    }

    close(fd);
    _unlink(); // 可以不借助信号量
    return 0;
}