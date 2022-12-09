#include "init.h"

void *sender1(void *arg);
void *sender2(void *arg);
void *receiver(void *arg);

int main()
{
    int res = -1;
    pthread_t p_t_send1;
    pthread_t p_t_send2;
    pthread_t p_t_receive;
    void *thread_result = NULL;

    //初始化信号量
    init();

    // 创建线程
    res = pthread_create(&p_t_send1, NULL, sender1, NULL);
    res = pthread_create(&p_t_send2, NULL, sender2, NULL);
    res = pthread_create(&p_t_receive, NULL, receiver, NULL);
    if (res != 0)
    {
        perror("pthread_create");
        exit(-1);
    }

    // 等待线程结束
    res = pthread_join(p_t_receive, &thread_result); // receiver线程结束前提是sender1和sender2线程都结束
    if (res != 0)
    {
        perror("pthread_join");
        exit(-1);
    }

    // 销毁信号量
    sem_close(sem_send);
    sem_close(sem_over1);
    sem_close(sem_over2);
    sem_close(sem_receive);

    sem_unlink("send");
    sem_unlink("over1");
    sem_unlink("over2");
    sem_unlink("receive");

    return 0;
}

void *sender1(void *arg)
{
    char str[100];
    int msgid;
    struct Msgbuf r_msg;
    struct Msgbuf s_msg;

    s_msg.msg_type = SEND;
    s_msg.tid = (unsigned int)pthread_self();
    s_msg.id = 1;

    sem_wait(sem_over1);
    msgid = msgget((key_t)1000, 0664 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("process(id:%d) tid: %u\n", s_msg.id, (unsigned int)pthread_self());

    while (1)
    {
        printf("input:\n");
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
    return NULL;
}

void *sender2(void *arg)
{
    char str[100];
    int msgid;
    struct Msgbuf r_msg;
    struct Msgbuf s_msg;

    s_msg.msg_type = SEND;
    s_msg.tid = (unsigned int)pthread_self();
    s_msg.id = 2;

    sem_wait(sem_over2);
    msgid = msgget((key_t)1000, 0664 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }
    printf("process(id:%d) tid: %u\n", s_msg.id, (unsigned int)pthread_self());

    while (1)
    {
        printf("input:\n");
        scanf("%[^\n]%*c", str);

        if (strcmp(str, "end2") == 0)
        {
            printf("please send \"exit\"\n");
            continue;
        }

        sem_wait(sem_send);

        if (strcmp(str, "exit") == 0)
        {
            strcpy(s_msg.text, "end2");
            msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
            sem_post(sem_receive);
            break;
        }

        strcpy(s_msg.text, str);
        msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);
        sem_post(sem_receive);
    }

    sem_wait(sem_over2);

    msgrcv(msgid, &r_msg, sizeof(struct Msgbuf), 0, 0);
    printf("receive message from %u:\n%s\n", r_msg.id, r_msg.text);

    sem_post(sem_send);
    return NULL;
}

void *receiver(void *arg)
{
    int msgid;
    struct Msgbuf r_msg;
    struct Msgbuf s_msg;
    int flag_over1 = 0;
    int flag_over2 = 0;

    msgid = msgget((key_t)1000, 0664 | IPC_CREAT);
    if (msgid == -1)
    {
        perror("msgget");
        exit(1);
    }

    sem_post(sem_over1);
    sem_post(sem_over2);

    s_msg.msg_type = RECEIVE;
    s_msg.tid = (unsigned int)pthread_self();
    s_msg.id = 3;
    printf("process(id:%d) tid: %u\n", s_msg.id, (unsigned int)pthread_self());

    while (1)
    {
        sem_wait(sem_receive);

        msgrcv(msgid, &r_msg, sizeof(struct Msgbuf), SEND, 0);
        printf("receive: message form sender(id:%d, tid:%u):  %s\n", r_msg.id, r_msg.tid, r_msg.text);

        if (r_msg.id == 1)
        {
            if (strcmp(r_msg.text, "end1") == 0)
            {
                printf("send message to sender(id:%d, thread %u): over\n", r_msg.id, r_msg.tid);
                strcpy(s_msg.text, "over");
                msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);

                sem_post(sem_over1);
                flag_over1 = 1;
            }
            else
            {
                sem_post(sem_send);
            }
        }
        else if (r_msg.id == 2)
        {
            if (strcmp(r_msg.text, "end2") == 0)
            {
                printf("send message to sender(id:%d, thread %u): over\n", r_msg.id, r_msg.tid);
                strcpy(s_msg.text, "over");
                msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);

                sem_post(sem_over2);
                flag_over2 = 1;
            }
            else
            {
                sem_post(sem_send);
            }
        }

        if (flag_over1 == 1 && flag_over2 == 1)
        {
            break;
        }
    }
}