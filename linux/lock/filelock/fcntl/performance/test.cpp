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

long gettimeuse(timeval& tv1, timeval&tv2)
{
    return tv2.tv_usec - tv1.tv_usec + 1000000*(tv2.tv_sec - tv1.tv_sec);
}

int lock_reg(int fd, int cmd, int type)
{
    struct flock lock;
    lock.l_type = type;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    
    return fcntl(fd, cmd, &lock);
}

void test_lockrt_unlockrt(int fd)
{
    int num = 100000;
    int ret;
    long timeuse, locktimeuse = 0, unlocktimeuse = 0;
    struct timeval tv_start, tv_end;

    for(int i = 0; i < num; i++)
    {
        gettimeofday(&tv_start, NULL);
        lock_reg(fd, F_SETLKW, F_WRLCK);
        gettimeofday(&tv_end, NULL);
        locktimeuse += gettimeuse(tv_start, tv_end);

        lock_reg(fd, F_SETLKW, F_UNLCK);
        gettimeofday(&tv_start, NULL);
        unlocktimeuse += gettimeuse(tv_end, tv_start);
    }
    printf("num=%d,locktimeuse=%ld,lockrt=%f,unlocktimeuse=%ld,unlockrt=%f\n", num, locktimeuse, (locktimeuse+0.0)/num, unlocktimeuse, (unlocktimeuse+0.0)/num);

}
void test_lockunlockrt(int fd)
{
    int num = 100000;
    long timeuse;
    struct timeval tv_start, tv_end;
    gettimeofday(&tv_start, NULL);
    for(int i = 0; i < num; i++)
    {
        lock_reg(fd, F_SETLKW, F_WRLCK);
        lock_reg(fd, F_SETLKW, F_UNLCK);
    }
    gettimeofday(&tv_end, NULL);
    timeuse = gettimeuse(tv_start, tv_end);
    printf("num=%d,lockunlocktime=%ld,lockunlockrt=%lf\n",
           num, timeuse, (timeuse+0.0)/num);
}

int main(int argc, char** argv)
{
    int fd = open("datafile", O_RDWR|O_CREAT);
    if(fd < 0)
    {
        printf("open file [%s] failed, error msg[%s]\n", "datafile", strerror(errno));
        return -1;
    }
    
    test_lockunlockrt(fd);
    test_lockrt_unlockrt(fd);
    
    return 0;
}

