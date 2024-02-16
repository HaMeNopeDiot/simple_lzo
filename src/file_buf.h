#ifndef FILE_BUF_H
#define FILE_BUF_H

#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_SIZE_BUFFER 100
#define DEFAULT_STEP_BUFFER 100

typedef struct {
   uint8_t* buf;
   size_t size_buf; 
} file_buf_t;

char* get_folder(char* full_path);
char* get_full_path(char* file, char* folder);
char* get_tag_file_name(char* pathname, char* tag, char* format_file, char separate_char);
void fd_check(FILE* fd);
int32_t file_buf_compare(file_buf_t first, file_buf_t second);
file_buf_t* file_buf_init(uint8_t* buf, size_t size_buf);
file_buf_t* file_buf_init_osize(size_t size_buf);
void file_buf_free(file_buf_t* obj);
void file_buf_printf(file_buf_t* obj);
file_buf_t* file_buf_read_file(char* file_path);
void file_buf_write_file(char* dst_path, file_buf_t* dst);
void file_buf_printf_check(file_buf_t *obj, char* name_obj);
size_t read_remain(FILE* file);
int cmp_files(char* path_file_a, char* path_file_b);

#endif /* FILE_BUF_H */