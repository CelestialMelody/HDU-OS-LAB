#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    // 读取 fifoname
    if (2 > argc)
    {
        printf("miss fifoname\n");
        exit(1);
    }

    // access: 判断文件是否存在
    // F_OK: 判断文件是否存在
    // 返回值: 0: 存在, -1: 不存在
    int ret = access(argv[1], F_OK);
    if (-1 == ret)
    {
        int ret = mkfifo(argv[1], 0664);
        if (-1 == ret)
        {
            perror("mkfifo");
            exit(1);
        }
        else
            printf("mkfifo success\n");
    }

    int fd = open(argv[1], O_RDONLY);
    if (-1 == fd)
    {
        perror("open");
        exit(1);
    }

    char buf[1024] = {0};

    int fifo_size = 0;

    // 方式一: 一直读取数据直到输入 quit 或者 exit
    while (1)
    {
        // 读取数据
        read(fd, buf, sizeof(buf));
        if (0 == strcmp(buf, "quit") || 0 == strcmp(buf, "exit"))
        {
            close(fd);
            unlink(argv[1]);
            exit(0);
        }
        printf("read: %s\n", buf);
    }

    // 方式二: 读取指定的字节数 直到管道为空
    // while (1)
    // {
    //     printf("please input the counts of chars you want to read:\n");
    //     int count = 0;
    //     if (!scanf("%d", &count))
    //     {
    //         close(fd);
    //         unlink(argv[1]);

    //         perror("scanf");
    //         exit(1);
    //     }

    //     memset(buf, 0, sizeof(buf));
    //     // read(fd, buf, sizeof(buf));

    //     // 设置读取的字节数
    //     read(fd, buf, count);
    //     printf("read: %s\n", buf);

    //     // count the fifo size
    //     ret = ioctl(fd, FIONREAD, &fifo_size); // FIONREAD: get the number of bytes in the FIFO
    //     if (-1 == ret)
    //     {
    //         close(fd);
    //         unlink(argv[1]);

    //         perror("ioctl");
    //         exit(1);
    //     }
    //     printf("fifo size = %d\n", fifo_size);

    //     // 当 buf 为空 或者 fifo_size 为 0 时，退出循环
    //     if (!strlen(buf) || fifo_size == 0)
    //     {
    //         break;
    //     }
    // }

    close(fd);
    unlink(argv[1]);
    return 0;
}
