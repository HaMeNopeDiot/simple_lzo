#include "lzo_shell.h"

#define DEFAULT_OUT_OVERWRITE_BUFFER ((16) * ((src->size_buf) / (1024)))

double get_time_in_seconds(clock_t begin, clock_t end)
{
    return (double)(end - begin) / CLOCKS_PER_SEC;
}
 
 char* int_to_text(int num) {
    int tmp = num;
    int size_num = 0;
    while(tmp != 0) {
        tmp /= 10;
        size_num++;
    }
    char* text_num = (char*)malloc(sizeof(char) * size_num + 1);
    sprintf(text_num, "%d", num);
    return text_num;
 }

int lzo_compress_switch(file_buf_t *src, file_buf_t *dst, void *wrkmem, int lzo_ver)
{
    int lzo1x_1_status;
    switch (lzo_ver) {
    case LZO_VERSION:
        printf("hel\n");
        lzo1x_1_status = lzorle1x_1_compress(src->buf, src->size_buf, dst->buf, &(dst->size_buf), (void*)(wrkmem));
        break;
    default:
        lzo1x_1_status = lzo1x_1_compress(src->buf, src->size_buf, dst->buf, &(dst->size_buf), (void*)(wrkmem));
        break;
    }
    return lzo1x_1_status;
}

char* lzo_gpathsize(char* output_path)
{
    char* size_file_tag = strdup(DEFAULT_SIZE_FILE_TAG);
    char* size_file_format = strdup(DEFAULT_SIZE_FILE_FORMAT);
    char* path_size_output = get_tag_file_name(output_path, size_file_tag, size_file_format, DEFAULT_SEPARATOR);
    free(size_file_tag);
    free(size_file_format);
    return path_size_output;
}

int lzo_compress(char* input_path, char* output_path, int lzo_ver)
{
    file_buf_t* src = file_buf_read_file(input_path);
    file_buf_t* dst = file_buf_init_osize(src->size_buf + DEFAULT_OUT_OVERWRITE_BUFFER);     //Size of dst can be more than src. That's why code allocate memory for dst more than src.
    uint8_t *wrkmem = calloc(LZO1X_MEM_COMPRESS, sizeof(uint8_t));
    int lzo1x_1_status = lzo_compress_switch(src, dst, (void*)(wrkmem), lzo_ver);
    verbose("@ Compression status: %d\n", lzo1x_1_status);
    file_buf_write_file(output_path, dst);
    // Write size buf
    char* path_size_output = lzo_gpathsize(output_path);
    verbose("Size file output path: %s\n", path_size_output);
    char* tmp_size = (char*)malloc(sizeof(char) * DEFAULT_SIZE_BUFFER);
    tmp_size = int_to_text(src->size_buf);
    file_buf_t* tmp = file_buf_init((uint8_t*)strdup(tmp_size), strlen(tmp_size));
    file_buf_write_file(path_size_output, tmp);
    // Free memory
    free(wrkmem);
    file_buf_free(src);
    file_buf_free(dst);
    file_buf_free(tmp);
    free(path_size_output);
    free(tmp_size);
    return lzo1x_1_status;
}

int lzo_decompress(char* input_path, char* output_path)
{

    file_buf_t* src = file_buf_read_file(input_path); // Try to read file
    //Find file and read size of source (not compressed file)
    char* size_file_tag = strdup(DEFAULT_SIZE_FILE_TAG);
    char* size_file_format = strdup(DEFAULT_SIZE_FILE_FORMAT);
    char* path_size_input = get_tag_file_name(input_path, size_file_tag, size_file_format, DEFAULT_SEPARATOR);
    file_buf_t* src_size = file_buf_read_file(path_size_input);
    size_t tmp_req = atoi((char*)src_size->buf);
    file_buf_t* dst = file_buf_init_osize(tmp_req);
    int lzo1x_1_dec_status = lzo1x_decompress_safe(src->buf, src->size_buf, dst->buf, &(dst->size_buf));
    verbose("@ Decompression status: %d\n", lzo1x_1_dec_status);
    file_buf_write_file(output_path, dst); // Input in file
    // Free memory
    file_buf_free(src);
    file_buf_free(src_size);
    file_buf_free(dst);
    free(size_file_format);
    free(size_file_tag);
    free(path_size_input);
    return lzo1x_1_dec_status;
}

int lzo_test(char* input_path, char* output_path, int lzo_ver)
{
    char* folder = get_folder(input_path);
    char* path_tmp = get_full_path(DEFAULT_COMP_OUT, folder);
    //Compressing
    clock_t begin_compress = clock();
    int comp_status = lzo_compress(input_path, path_tmp, lzo_ver);
    clock_t end_compress = clock();
    //Decompressing
    clock_t begin_decompress = clock();
    int dcmp_status = lzo_decompress(path_tmp, output_path);
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
    free(folder);
    free(path_tmp);
    return comp_status && dcmp_status ? 1: 0;
}