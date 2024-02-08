#include <stdio.h>
#include <time.h>
#include <string.h>

#include "lzo.h"

#include "file_buf.h"


void lzo_compress(file_buf_t* src, file_buf_t* dst)
{
    uint8_t *wrkmem = malloc(LZO1X_MEM_COMPRESS * sizeof(uint8_t));
    int lzo1x_1_status = lzo1x_1_compress(src->buf, src->size_buf, dst->buf, &(dst->size_buf), (void*)(wrkmem));
    printf("@ Compression status: %d\n", lzo1x_1_status);
    free(wrkmem);
}

void lzo_decompress(file_buf_t* src, file_buf_t* dst)
{
    int lzo1x_1_dec_status = lzo1x_decompress_safe(src->buf, src->size_buf, dst->buf, &(dst->size_buf));
    printf("@ Decompression status: %d\n", lzo1x_1_dec_status);
}

double get_time_in_seconds(clock_t begin, clock_t end)
{
    return (double)(end - begin) / CLOCKS_PER_SEC;
}

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
    return full_path;
}

#define DEFAULT_COMP_OUT "text_c.lzo"
#define DEFAULT_DECOMP_OUT "text_d.txt"

int main() 
{
    char* path_in = strdup("test/case-3/text.txt");

    char* folder = get_folder(path_in);

    char* path_comp = get_full_path(DEFAULT_COMP_OUT, folder);
    char* path_decc = get_full_path(DEFAULT_DECOMP_OUT, folder);
    
    file_buf_t* src = file_buf_read_file(path_in);
    file_buf_t* dst = file_buf_t_init(src->size_buf);

    //Compressing
    clock_t begin_compress = clock();
    lzo_compress(src, dst);
    clock_t end_compress = clock();

    //write dst in file
    file_buf_write_file(path_comp, dst);

    //read file to src2
    file_buf_t* src2 = file_buf_read_file(path_comp);
    file_buf_t* dec = file_buf_t_init(src->size_buf); //Size like a inital size

    // Decompress
    clock_t begin_decompress = clock();
    lzo_decompress(src2, dec);
    clock_t end_decompress = clock();
    
    //write dec in file
    file_buf_write_file(path_decc, dec);

    //Check if src and dec equal 
    int status_compare2 = file_buf_compare(*src, *dec);
    printf("status comparing: %d\n", status_compare2);
    
    // Free memory
    file_buf_free(src);
    file_buf_free(dst);
    file_buf_free(src2);
    file_buf_free(dec);
    free(path_in);
    free(path_comp);
    free(path_decc);
    //

    // Time counting
    double time_spent_compressing = get_time_in_seconds(begin_compress, end_compress);
    double time_spent_decompressing = get_time_in_seconds(begin_decompress, end_decompress);

    printf("@ Done!\nTime used for compress: %fs\nTime used for decompress: %fs\nTotal: %fs\n", time_spent_compressing, time_spent_decompressing, time_spent_compressing + time_spent_decompressing);
    
    return 0;
}