#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h> 
#include <iostream>

int clear_file_cache(const char *filename)   
{   
    struct stat st;   
    if(stat(filename , &st) < 0) {   
        fprintf(stderr , "stat localfile failed, path:%s\n",filename);   
        return -1;   
    }   

    int fd = open(filename, O_RDONLY);   
    if( fd < 0 ) {   
        fprintf(stderr , "open localfile failed, path:%s\n",filename);   
        return -1;   
    }   

    //clear cache by posix_fadvise  
    if( posix_fadvise(fd,0,st.st_size,POSIX_FADV_DONTNEED) != 0) {   
        printf("Cache FADV_DONTNEED failed, %s\n",strerror(errno));   
    }   
    else {   
        //printf("Cache FADV_DONTNEED done\n");   
    }   

    return 0;   
}  

void readfile(char* filename, int size,  bool isdirect)
{
    int fd;
    if(isdirect) {
        fd = open(filename, O_RDONLY | O_DIRECT);
    }
    else {
        fd = open(filename, O_RDONLY);
    }

    if(fd<0) {
        perror("open file failed\n");
        return ;
    }
    char* buf;
    posix_memalign((void**)&buf, 512, size); 

    int cnt = 0;
    while(true) {
        cnt = read(fd, buf, size);
        if(cnt >0) {
            //printf("%s", buf);
        }
        else if(cnt ==0) {
            break;
        }
        else {
            perror("read failed\n");
            break;
        }
    }
    return ;
}

int main(int argc, char**argv)
{
    char* filename = NULL;
    bool isdirect = false;
    if(argc < 2) {
        printf("usage: $0 file [isdirect]\n", argv[0]);
        return -1;
    }
    if(argc >= 2) {
        filename = argv[1];
    }
    if(argc >= 3) {
        isdirect = true;
    }
    for(int i = 8; i < 16; i++) {
        if(isdirect == false) {
            clear_file_cache(filename);
            sleep(1);
        }

        int size = i * 1024;
        timeval tvstart, tvend;
        gettimeofday(&tvstart, NULL);
        readfile(filename, size, isdirect);
        gettimeofday(&tvend, NULL);
        std::cerr << "size[" << size << "] time=" << tvend.tv_sec - tvstart.tv_sec << std::endl;
    }
    return 0;
}
