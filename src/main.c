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

int main() 
{
    clock_t begin_compress = clock();
    char* path_in = "test/case-4/text.txt";
    char* path_comp = "test/case-4/text_c.lzo";
    char* path_decc = "test/case-4/text_d.txt";
    
    file_buf_t* src = file_buf_read_file(path_in);
    file_buf_t* dst = file_buf_t_init(src->size_buf);

    //Compressing
    lzo_compress(src, dst);
    

    //src check
    //file_buf_printf_check(src, "src");

    //dst check
    //file_buf_printf_check(dst, "dst");

    //write dst in file
    file_buf_write_file(path_comp, dst);
    clock_t end_compress = clock();
    clock_t begin_decompress = clock();

    //read file to src2
    file_buf_t* src2 = file_buf_read_file(path_comp);

    //Check if this equal
    //int status_compare = file_buf_compare(*src2, *dst);
    //printf("status comparing: %d\n", status_compare);

    //Size like a inital size
    file_buf_t* dec = file_buf_t_init(src->size_buf);

    //src2 check
    //file_buf_printf_check(src2, "src2");

    // Decompress
    int lzo1x_1_dec_status = lzo1x_decompress_safe(src2->buf, src2->size_buf, dec->buf, &(dec->size_buf));
    printf("@ Decompression status: %d\n", lzo1x_1_dec_status);

    //dec check
    //file_buf_printf_check(dec, "dec");
    
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

    double time_spent_compressing = (double)(end_compress - begin_compress) / CLOCKS_PER_SEC;
    double time_spent_decompressing = (double)(end_decompress - begin_decompress) / CLOCKS_PER_SEC;

    printf("@ Done!\nTime used for compress: %f\nTime used for decompress: %f\nTotal: %f\n", time_spent_compressing, time_spent_decompressing, time_spent_compressing + time_spent_decompressing);
    
    return 0;
}