#include "init.h"

int main()
{
    char str[100];
    int msgid;
    struct Msgbuf r_msg; // 消息接受区
    struct Msgbuf s_msg;

    init();
    s_msg.msg_type = 1;
    s_msg.id = 1;

    sem_wait(sem_over1); // 确保receiver先初始化完成
    // 创建消息队列 see https://book.itheima.net/course/223/1277519158031949826/1277528201857343489
    msgid = msgget((key_t)1000, 0664 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("process tid: %u\n", (unsigned int)pthread_self());

    while (1)
    {
        printf("input:\n");
        // %[^\n]表示读取除换行符以外的所有字符
        // %*c表示读取一个字符并丢弃
        scanf("%[^\n]%*c", str);

        if (strcmp(str, "end1") == 0)
        {
            printf("please send \"exit\"\n");
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
    printf("receive message from %u:\n%s\n", r_msg.id, r_msg.text);

    sem_post(sem_send);
    return 0;
}