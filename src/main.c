#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "lzo.h"


#include "verbose.h"
#include "file_buf.h"
#include "parse_args.h"


#define DEFAULT_INPUT_FILE "text.txt"
#define DEFAULT_COMP_OUT "temporary_c.lzo"
#define DEFAULT_DECOMP_OUT "text_d.txt"
#define DEFAULT_SRC_SIZE_FILE "src_size.txt"

#define DEFAULT_SIZE_FILE_TAG "_size"
#define DEFAULT_SIZE_FILE_FORMAT "txt"
#define DEFAULT_SEPARATOR '.'

double get_time_in_seconds(clock_t begin, clock_t end)
{
    return (double)(end - begin) / CLOCKS_PER_SEC;
}
 
 /* reverse:  переворачиваем строку s на месте */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 /* itoa:  конвертируем n в символы в s */
 void itoa(int n, char *s)
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
         s[i++] = n % 10 + '0';   /* берем следующую цифру */
     } while ((n /= 10) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void lzo_compress(char* input_path, char* output_path)
{
    file_buf_t* src = file_buf_read_file(input_path);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    file_buf_t* dst = file_buf_t_init(src->size_buf);
    uint8_t *wrkmem = malloc(LZO1X_MEM_COMPRESS * sizeof(uint8_t));
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    int lzo1x_1_status = lzo1x_1_compress(src->buf, src->size_buf, dst->buf, &(dst->size_buf), (void*)(wrkmem));
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    verbose("@ Compression status: %d\n", lzo1x_1_status);
    file_buf_write_file(output_path, dst);
    // Write size buf
    char* size_file_tag = strdup(DEFAULT_SIZE_FILE_TAG);
    char* size_file_format = strdup(DEFAULT_SIZE_FILE_FORMAT);
    char* path_size_output = get_tag_file_name(output_path, size_file_tag, size_file_format, DEFAULT_SEPARATOR);
    verbose("Size file output path: %s\n", path_size_output);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    char* tmp_size = (char*)malloc(sizeof(char) * DEFAULT_SIZE_BUFFER);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    itoa(src->size_buf, tmp_size);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    file_buf_t* tmp = file_buf_t_init(strlen(tmp_size));
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    tmp->buf = (uint8_t*)strdup(tmp_size);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    file_buf_write_file(path_size_output, tmp);
    printf("size: %ld/%ld\n", strlen((char*)src->buf), src->size_buf);
    // Free memory
    printf("a\n");
    free(wrkmem);
    printf("a\n");
    //file_buf_printf_check(src, "src");
    file_buf_free(src);
    file_buf_free(dst);
    file_buf_free(tmp);
    free(size_file_tag);
    free(size_file_format);
    free(path_size_output);
    free(tmp_size);
}

void lzo_decompress(char* input_path, char* output_path)
{

    file_buf_t* src = file_buf_read_file(input_path);
    //Find file and read size of source (not compressed file)
    char* size_file_tag = strdup(DEFAULT_SIZE_FILE_TAG);
    char* size_file_format = strdup(DEFAULT_SIZE_FILE_FORMAT);
    char* path_size_input = get_tag_file_name(input_path, size_file_tag, size_file_format, DEFAULT_SEPARATOR);
    file_buf_t* src_size = file_buf_read_file(path_size_input);
    size_t tmp_req = atoi((char*)src_size->buf);
    file_buf_t* dst = file_buf_t_init(tmp_req);
    int lzo1x_1_dec_status = lzo1x_decompress_safe(src->buf, src->size_buf, dst->buf, &(dst->size_buf));
    verbose("@ Decompression status: %d\n", lzo1x_1_dec_status);
    file_buf_write_file(output_path, dst);
    // Free memory
    file_buf_free(src);
    file_buf_free(src_size);
    file_buf_free(dst);
    free(size_file_format);
    free(size_file_tag);
    free(path_size_input);
}

void lzo_test(char* input_path, char* output_path)
{
    char* folder = get_folder(input_path);
    char* path_tmp = get_full_path(DEFAULT_COMP_OUT, folder);
    //Compressing
    clock_t begin_compress = clock();
    lzo_compress(input_path, path_tmp);
    clock_t end_compress = clock();
    //Decompressing
    clock_t begin_decompress = clock();
    lzo_decompress(path_tmp, output_path);
    clock_t end_decompress = clock();
    // Time counting
    double time_spent_compressing = get_time_in_seconds(begin_compress, end_compress);
    double time_spent_decompressing = get_time_in_seconds(begin_decompress, end_decompress);
    int differences = cmp_files(input_path, output_path);
    verbose(
    "@ Done!\n"
    "Time used for compress: %fs\n"
    "Time used for decompress: %fs\nTotal: %fs\n" 
    "Differences in files: %d counts\n",
    time_spent_compressing, time_spent_decompressing, time_spent_compressing + time_spent_decompressing, differences
    );
}

int main(int argc, char *argv[])
{
    prs_args_t *init_obj = parse_args(argc, argv);
    setVerbose(init_obj->verbose);
    if(init_obj->input_file == NULL) {
        init_obj->input_file = strdup(DEFAULT_INPUT_FILE);
    }
    if(init_obj->output_file == NULL) {
        char* folder = get_folder(init_obj->input_file);
        if(init_obj->status == COMP_STATUS) {
            init_obj->output_file = get_full_path(DEFAULT_COMP_OUT, folder);
        } else {
            init_obj->output_file = get_full_path(DEFAULT_DECOMP_OUT, folder);
        }
    }
    verbose("INIT\nSource: %s\nOut: %s\nStatus: %d\n", init_obj->input_file, init_obj->output_file, init_obj->status);
    int status_prog = 0;
    switch (init_obj->status)
    {
    case DCMP_STATUS:
        lzo_decompress(init_obj->input_file, init_obj->output_file);
        break;
    case COMP_STATUS:
        lzo_compress(init_obj->input_file, init_obj->output_file);
        break;
    case TEST_STATUS:
        lzo_test(init_obj->input_file, init_obj->output_file);
        break;
    default:
        status_prog = 1;
        break;
    }
    prs_args_free(init_obj);
    return status_prog;
}