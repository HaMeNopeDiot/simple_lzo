#include <stdio.h>
#include <time.h>

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


int main() 
{
    clock_t begin_compress = clock();
    char* path_in = "test/case-3/text.txt";
    char* path_comp = "test/case-3/text_c.lzo";
    char* path_decc = "test/case-3/text_d.txt";
    
    file_buf_t* src = file_buf_read_file(path_in);
    file_buf_t* dst = file_buf_t_init(src->size_buf);

    //Compressing
    lzo_compress(src, dst);

    //write dst in file
    file_buf_write_file(path_comp, dst);
    clock_t end_compress = clock();
    clock_t begin_decompress = clock();

    //read file to src2
    file_buf_t* src2 = file_buf_read_file(path_comp);

    //Size like a inital size
    file_buf_t* dec = file_buf_t_init(src->size_buf);

    // Decompress
    lzo_decompress(src2, dec);
    
    //write dec in file
    file_buf_write_file(path_decc, dec);
    clock_t end_decompress = clock();

    //Check if src and dec equal 
    int status_compare2 = file_buf_compare(*src, *dec);
    printf("status comparing: %d\n", status_compare2);
    
    // Free memory
    file_buf_free(src);
    file_buf_free(dst);
    file_buf_free(src2);
    file_buf_free(dec);
    //

    // Time counting
    double time_spent_compressing = get_time_in_seconds(begin_compress, end_compress);
    double time_spent_decompressing = get_time_in_seconds(begin_decompress, end_decompress);

    printf("@ Done!\nTime used for compress: %fs\nTime used for decompress: %fs\nTotal: %fs\n", time_spent_compressing, time_spent_decompressing, time_spent_compressing + time_spent_decompressing);
    
    return 0;
}