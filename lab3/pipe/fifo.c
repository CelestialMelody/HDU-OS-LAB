#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>

int main(int argc, char *argv[])
{
    char buf[1024];
    char temp[256];
    int id;
    int fd;
    int ret;
    sem_t *mutex;
    sem_t *receive1;
    sem_t *receive2;
    sem_t *receive3;
    pid_t pid;

    if (argc < 2)
    { // 判断是否输入了文件名
        printf("miss fifoname\n");
        exit(1);
    }
    ret = access(argv[1], F_OK); // 判断文件是否存在
    if (-1 == ret)
    {
        int ret = mkfifo(argv[1], 0664); // 创建FIFO文件
        if (-1 == ret)
        {
            perror("mkfifo error");
            exit(1);
        }
        // else if (0 == ret)
        // {
        //     printf("mkfifo success");
        // }
    }

    memset(buf, 0, sizeof(buf)); // initialize buffer
    memset(temp, 0, sizeof(temp));
    sem_unlink("mutex"); // unlink semaphore if it exists
    sem_unlink("send1");
    sem_unlink("receive1");

    mutex = sem_open("mutex", O_CREAT, 0644, 1); // sem_open(name, flag, mode, value);
    receive1 = sem_open("receive1", O_CREAT, 0644, 0);
    receive2 = sem_open("receive2", O_CREAT, 0644, 0);
    receive3 = sem_open("receive3", O_CREAT, 0644, 0);

    for (id = 0; id < 3; id++)
    { // create 3 child processes
        pid = fork();
        if (0 == pid) // child process
        {
            break;
        }
        if (0 > pid)
        {
            sem_close(mutex);
            sem_close(receive1);
            sem_close(receive2);
            sem_close(receive3);

            sem_unlink("mutex");
            sem_unlink("receive1");
            sem_unlink("receive2");
            sem_unlink("receive3");

            perror("fork");
            exit(1);
        }
    }

    if (3 == id)
    {                                 // parent process
        fd = open(argv[1], O_RDONLY); // 需要同时获取
        sem_wait(receive1);           // wait for signal from child
        sem_wait(receive2);
        sem_wait(receive3);
        sem_wait(mutex); // lock

        read(fd, buf, sizeof(buf)); // read from pipe
        printf("parent read: %s\n", buf);

        // int size = 0;
        // ret = ioctl(fd, FIONREAD, &size);
        // if (-1 == ret)
        // {
        //     perror("ioctl");
        //     exit(1);
        // }
        // printf("fifo size is %d\n", size);

        // int count = 0;
        // while (1)
        // {
        //     memset(buf, 0, sizeof(buf));
        //     printf("please input the counts of chars you want to read:\n");
        //     if (!scanf("%d", &count))
        //     {
        //         sem_post(mutex); // unlock
        //         close(fd);

        //         sem_close(mutex);
        //         sem_close(receive1);
        //         sem_close(receive2);
        //         sem_close(receive3);

        //         sem_unlink("mutex");
        //         sem_unlink("receive1");
        //         sem_unlink("receive2");
        //         sem_unlink("receive3");
        //         unlink(argv[1]);

        //         perror("scanf");
        //         exit(1);
        //     }
        //     read(fd, buf, count); // read from pipe
        //     printf("Parent (pid:%d) receive: %s\n", getpid(), buf);

        //     ret = ioctl(fd, FIONREAD, &size);
        //     if (-1 == ret)
        //     {
        //         perror("ioctl");
        //         exit(1);
        //     }
        //     printf("fifo size = %d\n", size);

        //     printf("length of buf is %d\n", strlen(buf));
        // }

        sem_post(mutex); // unlock
        close(fd);

        sem_close(mutex);
        sem_close(receive1);
        sem_close(receive2);
        sem_close(receive3);

        sem_unlink("mutex");
        sem_unlink("receive1");
        sem_unlink("receive2");
        sem_unlink("receive3");
        unlink(argv[1]);
    }
    else if (0 == id)
    {                    // child process 1
        sem_wait(mutex); // lock
        fd = open(argv[1], O_WRONLY);
        printf("Child %d (pid:%d) send: ", id, getpid());
        // fgets(buf, sizeof(buf), stdin);
        memset(temp, 0, sizeof(temp));
        scanf("%[^\n]%*c", temp);
        sprintf(buf, "%s ", temp);
        write(fd, buf, strlen(buf)); // write to pipe
        close(fd);
        sem_post(mutex);    // unlock
        sem_post(receive1); // send signal to parent
    }
    else if (1 == id)
    { // child process 2
        sem_wait(mutex);
        fd = open(argv[1], O_WRONLY);
        printf("Child %d (pid:%d) send: ", id, getpid());
        scanf("%[^\n]%*c", temp);
        sprintf(buf, "%s ", temp);
        write(fd, buf, strlen(buf));
        close(fd);
        sem_post(mutex);
        sem_post(receive2);
    }
    else if (2 == id)
    { // child process 3
        sem_wait(mutex);
        fd = open(argv[1], O_WRONLY);
        printf("Child %d (pid:%d) send: ", id, getpid());
        scanf("%[^\n]%*c", temp);
        sprintf(buf, "%s ", temp);
        write(fd, buf, strlen(buf));
        close(fd);
        sem_post(mutex);
        sem_post(receive3);
    }

    sem_close(mutex);
    sem_close(receive1);
    sem_close(receive2);
    sem_close(receive3);

    sem_unlink("mutex");
    sem_unlink("send");
    sem_unlink("receive");

    return 0;
}