#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>

int main()
{
    int fd[2]; // file descriptor
    int ret;
    char buf[1024];
    int id;
    sem_t *mutex;
    sem_t *receive1;
    sem_t *receive2;
    sem_t *receive3;
    pid_t pid;

    memset(buf, 0, sizeof(buf)); // initialize buffer
    sem_unlink("mutex");         // unlink semaphore if it exists
    sem_unlink("receive1");
    sem_unlink("receive2");
    sem_unlink("receive3");

    mutex = sem_open("mutex", O_CREAT, 0644, 1); // sem_open(name, flag, mode, value);
    receive1 = sem_open("receive1", O_CREAT, 0644, 0);
    receive2 = sem_open("receive2", O_CREAT, 0644, 0);
    receive3 = sem_open("receive3", O_CREAT, 0644, 0);

    ret = pipe(fd); // create pipe; fd[0] = read end, fd[1] = write end
    if (ret == -1)
    {
        perror("pipe");
        exit(1);
    }

    for (id = 0; id < 3; id++)
    { // create 3 child processes
        pid = fork();
        if (0 == pid) // child process
        {
            // 使用这种方式创建 3 个子进程
            // 并且可以有效区分父子进程
            // id = 0, 1, 2 为子进程
            // id = 3 为父进程
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
    {                       // parent process
        close(fd[1]);       // close write end
        sem_wait(receive1); // wait for signal from child
        sem_wait(receive2);
        sem_wait(receive3);
        sem_wait(mutex); // lock

        // 打印管道大小
        int pipe_size = 0;
        ioctl(fd[0], FIONREAD, &pipe_size);
        printf("pipe size: %d bytes\n", pipe_size);
        while (pipe_size)
        {
            printf("please input the counts of chars you want to read:\n");
            int count = 0;

            if (!scanf("%d", &count))
            {
                perror("scanf");
                exit(1);
            }

            read(fd[0], buf, count); // read from pipe
            printf("Parent (pid:%d) receive: %s\n", getpid(), buf);

            // 打印管道大小
            ioctl(fd[0], FIONREAD, &pipe_size);
            printf("pipe size: %d bytes\n", pipe_size);
        }

        sem_post(mutex); // unlock

        sem_close(mutex);
        sem_close(receive1);
        sem_close(receive2);
        sem_close(receive3);

        sem_unlink("mutex");
        sem_unlink("send");
        sem_unlink("receive");
    }
    else if (0 == id)
    {                    // child process 1
        close(fd[0]);    // close read end
        sem_wait(mutex); // lock
        printf("Child %d (pid:%d) send: ", id, getpid());
        scanf("%[^\n]%*c", buf); // read from stdin, %[^\n]%*c : read until newline
        write(fd[1], buf, strlen(buf));
        sem_post(mutex);    // unlock
        sem_post(receive1); // send signal to parent
    }
    else if (1 == id)
    { // child process 2
        close(fd[0]);
        sem_wait(mutex);
        printf("Child %d (pid:%d) send: ", id, getpid());
        scanf("%[^\n]%*c", buf);
        write(fd[1], buf, strlen(buf));
        sem_post(mutex);
        sem_post(receive2);
    }
    else if (2 == id)
    { // child process 3
        close(fd[0]);
        sem_wait(mutex);
        printf("Child %d (pid:%d) send: ", id, getpid());
        scanf("%[^\n]%*c", buf);
        write(fd[1], buf, strlen(buf));
        sem_post(mutex);
        sem_post(receive3);
    }

    return 0;
}