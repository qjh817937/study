#include<errno.h>
#include<iostream>
#include<stdio.h>
#include<sys/mman.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

using namespace std;

int main(int argc, char **args)
{
    int len = 10;
    int fd = open(args[1], O_RDWR|O_CREAT,0777);
    // int fd = open(args[1], O_RDWR|O_CREAT);
    
    if(fd == -1)
    {
        printf("open [%s] failed\n",args[1]);
        return -1;
    } 
    lseek(fd, len, SEEK_SET);
    write(fd,"",1);  // 如果文件是刚创建的，则必须加一这句话，为什么????
    
    char *p;
    p = (char*) mmap(0, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if(MAP_FAILED == p)
    {
        printf("map failed, errno is [%d]\n",errno);
        printf("EACCES[%d],EAGAIN[%d],EBADF[%d],EINVAL[%d], EMFILE[%d],   \n",EACCES,EAGAIN,EBADF,EINVAL,EMFILE);
        return -1;
    }
    

    for(int i = 0; i < len; i++)
    {
        printf("p[%d]=[%c]\n",i , p[i]);
    }
    
    return 0;
}
