#include "init.h"

int main()
{
    // char str[100]; // bug
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
        char str[100] = {0}; // fix bug
        printf("input:\n");

        // used
        // see https://stackoverflow.com/questions/30065675/what-does-scanf-nc-mean
        // scanf("%[^\n]%*c", str);

        fgets(str, 100, stdin); // 会读取空格与换行符

        // used
        // if (strcmp(str, "end1") == 0)
        if (strncmp(str, "end1", 4) == 0)
        {
            printf("please send \"exit\"\n");
            continue;
        }

        sem_wait(sem_send);

        // used
        // if (strcmp(str, "exit") == 0)

        if (strncmp(str, "exit", 4) == 0)
        {
            strcpy(s_msg.text, "end1");
            msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
            sem_post(sem_receive);
            break;
        }

        // used
        // strcpy(s_msg.text, str);

        // str 丢弃换行符后再发送
        str[strlen(str) - 1] = '\0'; // fix bug
        // strncpy(s_msg.text, str, strlen(str) - 1); // bug 不会清空s_msg.text
        strcpy(s_msg.text, str);

        // test
        printf("sender1 str len: %ld\n", strlen(str));
        printf("s_msg.text len: %ld\n", strlen(s_msg.text)); // 发现没有清空

        msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
        sem_post(sem_receive);
    }
    sem_wait(sem_over1);

    msgrcv(msgid, &r_msg, sizeof(struct Msgbuf), 0, 0);
    printf("receive message from %u:\n%s\n", r_msg.id, r_msg.text);

    sem_post(sem_send);
    return 0;
}