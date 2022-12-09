/**
 * shm1 与 shm2 通过共享内存通信
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

#include <pthread.h>
#include <semaphore.h> //信号量头文件
#include <glob.h>

#define MEM_MIN_SIZE 1024

sem_t *sem_sender;
sem_t *sem_receiver1;
sem_t *sem_receiver2;

key_t key;
int shm_id;
void *shm_p;

void init()
{
    //初始化信号量
    sem_sender = sem_open("sender", O_CREAT, 0666, 1);
    sem_receiver1 = sem_open("receiver1", O_CREAT, 0666, 0);
    sem_receiver2 = sem_open("receiver2", O_CREAT, 0666, 0);

    key = ftok("/home/melody/lab/3lab/", 0);
    if (key == -1)
    {
        perror("ftok error");
        exit(-1);
    }
    printf("key = %d\n", key);

    //创建共享内存，成功则返回id (一个与key相关的标识符)
    shm_id = shmget(key, MEM_MIN_SIZE, 0666 | IPC_CREAT);
    if (shm_id < 0)
    {
        perror("shmget");
        exit(-1);
    }
    printf("shm_id = %d\n", shm_id);

    //指定共享内存映射到新虚拟地址空间，返回起始地址
    shm_p = shmat(shm_id, NULL, 0); // 0表示可读可写
    if (shm_p == (void *)-1)
    {
        perror("shmat");
        exit(-1);
    }
    return;
}