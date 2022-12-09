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

sem_t *sem_send;
sem_t *sem_receive;
sem_t *sem_over1;
sem_t *sem_over2;

// 缓冲区mymsg
struct Msgbuf
{
    long msg_type; // 消息类型1为发送者的消息，2为接收的消息。
    int id;        // 1为发送者1，2为发送者2, 3为接收者
    char text[100];
};

void init()
{
    // 初始化信号量
    sem_send = sem_open("send", O_CREAT, 0666, 1);
    sem_receive = sem_open("receive", O_CREAT, 0666, 0);
    sem_over1 = sem_open("over1", O_CREAT, 0666, 0);
    sem_over2 = sem_open("over2", O_CREAT, 0666, 0);
}