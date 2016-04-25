#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>

sem_t *sem;

long gettimeuse(timeval& tv1, timeval&tv2)
{
    return tv2.tv_usec - tv1.tv_usec + 1000000*(tv2.tv_sec - tv1.tv_sec);
}

void* thread_fun(void* argv)
{
    int num = 100000;
    if(argv != NULL)
        num = *(int*)argv;

    struct timeval tv1,tv2;
    gettimeofday(&tv1, NULL);
    for(int i = 0; i < num; i++)
    {
        sem_wait(sem);
        sem_post(sem);
    }
    gettimeofday(&tv2, NULL);

    long timeuse = gettimeuse(tv1,tv2);
    printf("num=%d,timeuse=%ld,lockunlockrt=%lf(us)\n", num, timeuse, (timeuse+0.0)/num);    
}

void test_multithread_performance()
{
    const int num = 10;
    pthread_t tids[num];

    for(int i = 1; i <= num; i++)
    {
        printf("thread num=%d\n", i);
        for(int j = 0; j <i; j++)
        {
            pthread_create(&tids[j], NULL, thread_fun, NULL);
        }
        for(int j = 0; j <i; j++)
        {
            pthread_join(tids[j], NULL);
        }
    }
}

void test_performance()
{
    int num = 200000000;
    thread_fun(&num);
}

int main(int argc, char** argv)
{
    sem = sem_open("testsem", O_CREAT, 0644, 1);
    if(argc > 2)
    {
        test_multithread_performance();
    }
    else
    {
        test_performance();
    }

    sem_close(sem);
    sem_unlink("testsem"); 
    return 0;
}
