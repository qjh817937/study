#include <semaphore.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>

sem_t sem;

long gettimeuse(timeval& tv1, timeval&tv2)
{
    return tv2.tv_usec - tv1.tv_usec + 1000000*(tv2.tv_sec - tv1.tv_sec);
}

void* thread_fun(void* argv)
{
    struct timeval tv1,tv2;
    gettimeofday(&tv1, NULL);
    int num = 100000;
    for(int i = 0; i < num; i++)
    {
        sem_wait(&sem);
        sem_post(&sem);
    }
    gettimeofday(&tv2, NULL);

    long timeuse = gettimeuse(tv1,tv2);
    printf("num=%d,timeuse=%ld,lockunlockrt=%lf(us)\n", num, timeuse, (timeuse+0.0)/num);    
}

void test_performance()
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

int main()
{
    sem_init(&sem, 0, 1);
    test_performance();
}
