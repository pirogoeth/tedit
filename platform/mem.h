#ifndef MEM_H
#define MEM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void *mem_filemap(FILE *fobj);
void *mem_fileremap(FILE *fobj, void *map, size_t asize);
int mem_fileunmap(FILE *fobj, void *map);

#endif