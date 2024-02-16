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

char* get_folder(char* full_path)
{
    char* folder_tmp = strdup(full_path);
    char* p_slash = folder_tmp;
    for(size_t i = 0; i < strlen(folder_tmp); i++) {
        if(folder_tmp[i] == '/') 
            p_slash = folder_tmp + i + 1;
    }
    size_t len_folder_string = p_slash - folder_tmp + 1;
    char* folder = (char*)malloc(sizeof(char) * (len_folder_string));
    for(size_t i = 0; i < len_folder_string - 1; i++) {
        folder[i] = folder_tmp[i];
    }
    folder[len_folder_string - 1] = '\0';
    free(folder_tmp);
    return folder;
}

char* get_full_path(char* file, char* folder)
{
    char* full_path = (char*)malloc(sizeof(char) * (strlen(file) + strlen(folder) + 1));
    char* ptr_o = full_path;
    char* ptr_i = folder;
    while(ptr_i != folder + strlen(folder)) {
        *(ptr_o++) = *(ptr_i++);
    }
    ptr_i = file;
    while(ptr_i != file + strlen(file)) {
        *(ptr_o++) = *(ptr_i++);
    }
    *(ptr_o) = '\0';
    return full_path;
}

// test_folder/abc.txt + _d -> test_folder/abc_d.txt
char* get_tag_file_name(char* pathname, char* tag, char* format_file, char separate_char)
{
    size_t nf_size = strlen(pathname) + strlen(tag) + 1;
    char* nf = (char*)malloc(sizeof(char) * (nf_size));
    char* ps = nf;
    char* pp = pathname;
    char* pt = tag;
    char* ppoint = pp;
    char* pf = format_file;
    for(size_t i = 0; i < strlen(pathname); i++) {
        if(pathname[i] == separate_char) {
            ppoint = pathname + i;
        }
    }
    if(*ppoint != separate_char) {
        ppoint = pathname + strlen(pathname) - 1;
    } 
    while(pp != ppoint) {
        *(ps++) = *(pp++);
    }
    while(*pt != '\0') {
        *(ps++) = *(pt++);
    }
    if(*ppoint == '.') 
    {   *(ps++) = *ppoint; 
        while(*pf != '\0') {
            *(ps++) = *(pf++);
        }
    }
    *(ps) = '\0';
    return nf;
}


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
    fbt->buf = calloc(size_buf, sizeof(uint8_t));
    fbt->size_buf = size_buf;
    return fbt;
}


void file_buf_free(file_buf_t* obj)
{
    //printf("size: %ld/%ld\n", strlen((char*)obj->buf), obj->size_buf);
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
    //printf("buf: %ld\n", i);
    fclose(fd);
    return init_file_buf_t(buf, i);
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

size_t read_remain(FILE* file)
{
    size_t length = 0;
    while(!feof(file)) { // ANY FILE WILL BE HERE AT ONCE!
        fgetc(file);
        length++;
    }
    return length - 1;
}


int cmp_files(char* path_file_a, char* path_file_b)
{
    int differences = 0;
    FILE *fd_a = fopen(path_file_a, "rb");
    fd_check(fd_a);
    FILE *fd_b = fopen(path_file_b, "rb");
    fd_check(fd_b);
    char a,b;
    while(!feof(fd_a) && !feof(fd_b)) {
        a = fgetc(fd_a);
        b = fgetc(fd_b);
        differences += a == b? 0 : 1;
    }
    if(!feof(fd_a)) {
        differences += read_remain(fd_a);
    }
    if(!feof(fd_b)) {
        differences += read_remain(fd_b);
    }
    fclose(fd_a);
    fclose(fd_b);
    return differences;
}


#endif /* FILE_BUF_H */