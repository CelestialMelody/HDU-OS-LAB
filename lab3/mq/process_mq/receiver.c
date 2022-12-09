// receiver.c
#include "init.h"

int main()
{
    int msgid;
    int flag_over1 = 0;
    int flag_over2 = 0;
    struct Msgbuf r_msg; //消息接受区
    struct Msgbuf s_msg;

    msgid = msgget((key_t)1000, 0666 | IPC_CREAT);

    init();

    sem_post(sem_over1);
    sem_post(sem_over2);

    s_msg.msg_type = 2;
    s_msg.id = 3;

    printf("process tid: %u\n", (unsigned int)pthread_self());

    while (1)
    {
        sem_wait(sem_receive);
        msgrcv(msgid, &r_msg, sizeof(struct Msgbuf), 0, 0);
        printf("receive message from %u:\n%s\n", r_msg.id, r_msg.text);

        if (r_msg.id == 1)
        {
            if (strcmp(r_msg.text, "end1") == 0)
            {
                printf("sending message to process 1: over\n");
                strcpy(s_msg.text, "over1");
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
                printf("sending message to process 2: over\n");
                strcpy(s_msg.text, "over2");
                msgsnd(msgid, &s_msg, sizeof(struct Msgbuf), 0);

                sem_post(sem_over2);
                flag_over2 = 1;
            }
            else
            {
                sem_post(sem_send);
            }
        }

        if (flag_over1 && flag_over2)
            break;
    }

    sem_unlink("send");
    sem_unlink("receive");
    sem_unlink("over1");
    sem_unlink("over2");

    return 0;
}