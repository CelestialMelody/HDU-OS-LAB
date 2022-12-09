#include "init.h"

int main()
{
    char str[100];
    int msgid;
    struct Msgbuf r_msg; //消息接受区
    struct Msgbuf s_msg;

    init();

    fd1 = open(fifofile1, O_RDONLY);
    sem_wait(sem_over1);
    read(fd1, &msgid, sizeof(int));

    s_msg.msg_type = 1;
    s_msg.id = 1;

    printf("tid:%u 进程(线程)1: \n", (unsigned int)pthread_self());

    while (1)
    {
        printf("发送：");
        scanf("%[^\n]%*c", str);

        if (strcmp(str, "end1") == 0)
        {
            printf("退出请发送：exit\n");
            continue;
        }

        sem_wait(sem_send);

        if (strcmp(str, "exit") == 0)
        {
            strcpy(s_msg.text, "end1");
            msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
            sem_post(sem_receive);
            break;
        }

        strcpy(s_msg.text, str);
        msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
        sem_post(sem_receive);
    }
    sem_wait(sem_over1);

    msgrcv(msgid, &r_msg, sizeof(struct Msgbuf), 0, 0);
    printf("收到线程%u的消息: %s\n", r_msg.id, r_msg.text);

    sem_post(sem_send);
    return 0;
}