#include "init.h"
int main()
{
    sem_unlink("send");
    sem_unlink("receive");
    sem_unlink("over1");
    sem_unlink("over2");

    unlink(fifofile1);
    unlink(fifofile2);
    return 0;
}