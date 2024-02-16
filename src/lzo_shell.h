#ifndef LZO_SHELL_H
#define LZO_SHELL_H

#define DEFAULT_SRC_SIZE_FILE "src_size.txt"

#define DEFAULT_INPUT_FILE "text.txt"
#define DEFAULT_COMP_OUT "temporary_c.lzo"
#define DEFAULT_DECOMP_OUT "text_d.txt"

#define DEFAULT_SIZE_FILE_TAG "_size"
#define DEFAULT_SIZE_FILE_FORMAT "txt"
#define DEFAULT_SEPARATOR '.'

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "lzo.h"
#include "verbose.h"
#include "file_buf.h"
#include "parse_args.h"
#include "lzo1x_compress.h"
#include "lzo1x_decompress_safe.h"

double get_time_in_seconds(clock_t begin, clock_t end);
int lzo_compress(char* input_path, char* output_path);
int lzo_decompress(char* input_path, char* output_path);
int lzo_test(char* input_path, char* output_path);


#endif /* LZO_SHELL_H */