#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <pthread.h>
#include <semaphore.h>

#define SEND 1
#define RECEIVE 2

int fd1, fd2; //消息队列标识符

sem_t *sem_send;
sem_t *sem_receive;
sem_t *sem_over1;
sem_t *sem_over2;

//缓冲区mymsg
struct Msgbuf
{
    long msg_type;
    int id;  // 1为发送者1，2为发送者2。
    int tid; //发送者的线程id
    char text[100];
};

void init()
{
    //初始化信号量
    sem_send = sem_open("send", O_CREAT, 0666, 1);
    sem_receive = sem_open("receive", O_CREAT, 0666, 0);
    sem_over1 = sem_open("over1", O_CREAT, 0666, 0);
    sem_over2 = sem_open("over2", O_CREAT, 0666, 0);
}