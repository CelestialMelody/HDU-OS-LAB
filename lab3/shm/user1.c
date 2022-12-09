#include "init.h"

// 用于
pthread_t r_thread, s_thread;

void *send(void *arg)
{
    char msg[MEM_MIN_SIZE]; // msg -> fmt string
    char s_str[100];        // input string
    while (1)
    {
        printf("Process %d input:\n", *((pid_t *)arg));
        fflush(stdout);
        // %[^\n] 指定输入的字符串中不包含换行符
        // %*c 读取并丢弃一个字符
        scanf("%[^\n]%*c", s_str);

        // Wait for the receiver to be ready.
        sem_wait(sem_sender);

        sprintf(msg, "%d:%s", *((pid_t *)arg), s_str);
        strcpy((char *)shm_p, msg);

        // Tell the receiver(user2) that the message is ready.
        sem_post(sem_receiver2);

        // send "over" to exit.
        if (!strcmp(s_str, "over"))
        {
            pthread_cancel(r_thread); // Cancel the receiver thread.
            shmdt(shm_p);
            break;
        }
    }
}

void *receive(void *arg)
{
    char r_str[100];
    char *p;
    while (1)
    {
        // Wait to receive a message from the sender(user2).
        sem_wait(sem_receiver1);

        strcpy(r_str, (char *)shm_p);

        // Find the first ':' in the string.
        p = strchr(r_str, ':');
        *(p++) = '\0';
        printf("Received message from process %s:\n%s\n", r_str, p);

        // if user1 send "over" to exit.
        // see https://book.itheima.net/course/223/1277519158031949826/1277528201861537793
        if (strcmp(p, "over") == 0)
        {
            pthread_cancel(s_thread);       // Cancel the sender thread.
            shmdt(shm_p);                   // Detach the shared memory.
            shmctl(shm_id, IPC_RMID, NULL); // Remove the shared memory.
            sem_unlink("sender");           // Remove the semaphore.
            sem_unlink("receiver1");
            sem_unlink("receiver2");
            break;
        }

        printf("Process %d input:\n", *((pid_t *)arg));
        fflush(stdout);
        sem_post(sem_sender);
    }
}

int main()
{
    pid_t pid = getpid();
    int res1 = 0, res2 = 0;

    init();

    printf("Process %d Start...\n\n", pid);
    // pthread_create() see https://book.itheima.net/course/223/1277519158031949826/1277528575729213444
    res1 = pthread_create(&s_thread, NULL, send, &pid);
    res2 = pthread_create(&r_thread, NULL, receive, &pid);

    if (res1 || res2)
    {
        printf("create pthread failed\n");
        return 1;
    }

    pthread_join(s_thread, NULL);
    pthread_join(r_thread, NULL);
    printf("Process %d Exit...\n", pid);
}