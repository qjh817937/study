#include <semaphore.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

char semname[] = "named_sem";
sem_t *sem;

bool use_sem = false;
long sum = 1000000;
long num = 0;

void test_sem_open()
{
    {
        sem = sem_open(semname, O_CREAT, 0644, 1);
        if(SEM_FAILED == sem)
        {
            printf("open sem[%s] with O_CREAT, failed, errormsg[%s]\n", semname, strerror(errno));
            return ;
        }
        else
        {
            printf("open sem[%s] with O_CREAT succ\n", semname);
        }
        
        sem = sem_open(semname, O_CREAT|O_EXCL, 0644, 1);
        if(SEM_FAILED == sem)
        {
            printf("open sem[%s] with O_CREAT|O_EXCL failed, errormsg[%s]\n", semname, strerror(errno));
        }
        else
        {
            printf("open sem[%s] with O_CREAT|O_EXCL succ\n", semname);
        }
    }
    {
        sem = sem_open(semname, O_CREAT, 0644, 1);
        if(SEM_FAILED == sem)
        {
            printf("open sem[%s] with O_CREAT failed, errormsg[%s]\n", semname, strerror(errno));
        }
        else
        {
            printf("open sem[%s] with O_CREAT succ\n", semname);
        }
        sem_close(sem);
        sem_unlink(semname); // remove
        
        sem = sem_open(semname, O_CREAT|O_EXCL, 0644, 1);
        if(SEM_FAILED == sem)
        {
            printf("open sem[%s] with O_CREAT|O_EXCL failed, errormsg[%s]\n", semname, strerror(errno));
        }
        else
        {
            printf("open sem[%s] with O_CREAT|O_EXCL succ\n", semname);
        }
    }
}

void* thread_fun_one(void* argv)
{
    for(int i = 0; i < sum; i++)
    {
        if(use_sem)
        {
            sem_wait(sem);
            num++;
            sem_post(sem);
        }
        else
        {
            num++;
        }
    }
}

void* thread_fun_two(void* argv)
{
    for(int i = 0; i < sum; i++)
    {
        if(use_sem)
        {
            sem_wait(sem);
            num--;
            sem_post(sem);
        }
        else
        {
            num--;
        }
    }
}

void test_sem_wait_post_use_as_mutex()
{
    const int num = 2;
    pthread_t tids[num];
    printf("thread num=%d\n", num);
    for(int i = 0; i < num; i++)
    {
        if(i==0)
        {
            pthread_create(&tids[i], NULL, thread_fun_one, NULL);
        }
        else
        {
            pthread_create(&tids[i], NULL, thread_fun_two, NULL);
        }
    }

    for(int i = 0; i < num; i++)
    {
        pthread_join(tids[i], NULL);
    }    
}

void test_sem_wait_post()
{
    {
        sem = sem_open(semname, O_CREAT, 0644, 1);

        use_sem = false;
        num = 0;
        test_sem_wait_post_use_as_mutex();
        assert(num != 0);

        use_sem = true;
        num = 0;
        test_sem_wait_post_use_as_mutex();
        assert(num == 0);
    }
}

int main(int argc, char** argv)
{
    test_sem_open();
    test_sem_wait_post();
    return 0;
}
