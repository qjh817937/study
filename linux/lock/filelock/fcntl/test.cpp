#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string>

struct threadarg
{
    int fd;
    int sleeptime;
    int num;
};

std::string gettime()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    char buf[20];
    sprintf(buf, "%ld.%ld", tv.tv_sec, tv.tv_usec);
    return std::string(buf);
}

void set_lock(int fd, int cmd, int type)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    int ret = fcntl(fd, cmd, &lock);
    gettimeofday(&tv_end, NULL);
    long timeuse = tv_end.tv_usec - tv_start.tv_usec + 1000000*(tv_end.tv_sec - tv_start.tv_sec);
    if(fcntl(fd, cmd, &lock) != 0)
    {
        printf("%s pid[%d] tid[%d] cmd[%d] type [%d] timeuse[%ld] failed, errormsg[%s]\n",gettime().c_str(), getpid(), pthread_self(), cmd, type, timeuse, strerror(errno));
    }
    else
    {
        printf("%s pid[%d] tid[%d] cmd[%d] type [%d] timeuse[%ld] succ\n", gettime().c_str(), getpid(), pthread_self(), cmd, type, timeuse);
    }
}

void* print_thread_fun(void *argv)
{
    threadarg targ = *(threadarg*)argv;
    while(true)
    {
        usleep(500000);
        printf("%s pid[%d] tid[%d] print\n", gettime().c_str(), getpid(), pthread_self());
    }
}

void* test_setlk_thread_fun(void *argv)
{
    threadarg targ = *(threadarg*)argv;
    for(int i = 0; i < targ.num; i++)
    {
        set_lock(targ.fd, F_SETLK, F_WRLCK);
        usleep(targ.sleeptime);
        set_lock(targ.fd, F_SETLK, F_UNLCK);
        usleep(targ.sleeptime);
    }
}


void test_setlk(int fd)
{
    pthread_t tid1, tid2, tid3;
    threadarg targ1;
    targ1.fd = fd;
    targ1.sleeptime = 1000000;
    targ1.num = 20;
    pthread_create(&tid1, NULL, test_setlk_thread_fun, &targ1);

    threadarg targ2;
    targ2.fd = fd;
    targ2.sleeptime = 10000000;
    targ2.num = 10;
    pthread_create(&tid2, NULL, test_setlk_thread_fun, &targ2);

    threadarg targ3;
    pthread_create(&tid3, NULL, print_thread_fun, &targ3);
    
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
}

void test_setlkw()
{

}

void test_getlk()
{
}

int main(int argc, char** argv)
{
    int fd = open("datafile", O_RDWR|O_CREAT);
    if(fd < 0)
    {
        printf("open file [%s] failed, error msg[%s]\n", "datafile", strerror(errno));
        return -1;
    }

    test_setlk(fd);
    return 0;
}

