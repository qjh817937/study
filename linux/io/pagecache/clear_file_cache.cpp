#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h> 

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
        printf("Cache FADV_DONTNEED done\n");   
    }   

    return 0;   
}  

int main(int argc, char**argv)
{
    if(argc < 2) {
        printf("usage: %s file", argv[0]);
        return -1;
    }

    return clear_file_cache(argv[1]);
}
