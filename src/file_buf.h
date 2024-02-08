#ifndef FILE_BUF_H
#define FILE_BUF_H

#include "types.h"
#include <stdlib.h>

#define DEFAULT_SIZE_BUFFER 100
#define DEFAULT_STEP_BUFFER 100

typedef struct {
   uint8_t* buf;
   size_t size_buf; 
} file_buf_t;


void fd_check(FILE* fd)
{
    if(fd == NULL) {
        printf("Incorrect path/name file!\n");
        exit(1);
    }
} 

int32_t file_buf_compare(file_buf_t first, file_buf_t second)
{
    if(first.size_buf != second.size_buf) {
        return (-1) * abs(first.size_buf - second.size_buf);
    } else {
        int32_t difference = 0;
        for(size_t i = 0; i < first.size_buf; i++) {
            if(first.buf[i] != second.buf[i]) {
                difference++;
            }
        }
        return difference;
    }
}

file_buf_t* init_file_buf_t(uint8_t* buf, size_t size_buf)
{
    file_buf_t *fbt = (file_buf_t*)malloc(sizeof(file_buf_t));
    fbt->buf = buf;
    fbt->size_buf = size_buf;
    return fbt;
}

file_buf_t* file_buf_t_init(size_t size_buf)
{
    file_buf_t *fbt = (file_buf_t*)malloc(sizeof(file_buf_t));
    fbt->buf = malloc(sizeof(uint8_t) * size_buf);
    fbt->size_buf = size_buf;
    return fbt;
}

void file_buf_free(file_buf_t* obj)
{
    free(obj->buf);
    free(obj);
}

void file_buf_printf(file_buf_t* obj)
{
    for(size_t i = 0; i < obj->size_buf; i++) {
        printf("%d", (uint8_t)obj->buf[i]);
    }
    printf("\n");
}

file_buf_t* file_buf_read_file(char* file_path)
{
    FILE* fd = fopen(file_path, "rb");
    fd_check(fd);
    uint8_t *buf = (uint8_t*)malloc((DEFAULT_SIZE_BUFFER) * sizeof(uint8_t));
    size_t buf_sz = DEFAULT_SIZE_BUFFER;
    size_t i = 0;
    while(!feof(fd)) {
        buf[i] = fgetc(fd);
        i++;
        if(i >= buf_sz) {
            buf_sz += DEFAULT_STEP_BUFFER;
            void *tmp = buf;
            buf = (uint8_t*)realloc(buf, (size_t)(buf_sz * sizeof(uint8_t)));
            if(buf == NULL) { 
                free(tmp); exit(1); 
            }
        }
    }
    buf[i] = '\0';
    //printf("buf: %s\n", buf);
    fclose(fd);
    return init_file_buf_t(buf, i - 1);
}

void file_buf_write_file(char* dst_path, file_buf_t* dst)
{
    FILE *fd = fopen(dst_path, "wb");
    fd_check(fd);
    size_t i;
    char c_tmp;
    for(i = 0; i < dst->size_buf; i++)  {
        c_tmp = dst->buf[i]; 
        fwrite(&c_tmp, sizeof(char), 1, fd);
    }
    fclose(fd);
}

void file_buf_printf_check(file_buf_t *obj, char* name_obj)
{
    printf("%s with size: %lu\n", name_obj, obj->size_buf);
    file_buf_printf(obj);
    printf("\n");
}


#endif /* FILE_BUF_H */