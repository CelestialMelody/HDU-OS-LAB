#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <pthread.h>
#include <semaphore.h>

sem_t *sem_send;
sem_t *sem_receive;
sem_t *mutex;

void init()
{
    // 初始化信号量
    sem_send = sem_open("send", O_CREAT, 0666, 1);
    sem_receive = sem_open("receive", O_CREAT, 0666, 0);
    mutex = sem_open("mutex", O_CREAT, 0666, 1);
}

void _unlink()
{
    sem_unlink("send");
    sem_unlink("receive");
    sem_unlink("mutex");
}