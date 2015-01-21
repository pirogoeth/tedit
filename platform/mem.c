#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "mem.h"

void *mem_filemap(FILE *fobj) {
    fseek(fobj, 0L, SEEK_END);
    size_t fsize = ftell(fobj);
    rewind(fobj);

    return mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_PRIVATE, fobj->_file, 0L);
}


void *mem_fileremap(FILE *fobj, void *map, size_t asize) {
    fseek(fobj, 0L, SEEK_END);
    size_t fsize = ftell(fobj);
    rewind(fobj);

#if defined __FreeBSD__ || defined __APPLE__
    munmap(map, fsize);
    return mmap(NULL, asize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_PRIVATE, fobj->_file, 0L);
#elif defined __linux__
    void *nmap = mremap(map, fsize, asize, MREMAP_MAYMOVE);
    if (nmap == MAP_FAILED) {
        abort();
    }
    return nmap
#endif
}

int mem_fileunmap(FILE *fobj, void *map) {
    fseek(fobj, 0L, SEEK_END);
    size_t fsize = ftell(fobj);
    rewind(fobj);

    return munmap(map, fsize);
}

int main(int argc, char *argv[]) {
    FILE *fobj = fopen("/usr/home/pirogoeth/tedit/platform/test.txt", "r+");
    
    void *map = mem_filemap(fobj);
    return mem_fileunmap(fobj, map);
}
