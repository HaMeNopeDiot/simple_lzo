#ifndef LZO1X_COMPRESS_H
#define LZO1X_COMPRESS_H

#include <stdlib.h>
#include <stdio.h>

#include "lzodefs.h"
#include "types.h"
#include "libfdt_env.h"
#include "lzo.h"
#include "align.h"

size_t lzo1x_1_do_compress(const unsigned char *in, size_t in_len,
		    unsigned char *out, size_t *out_len,
		    size_t ti, void *wrkmem, signed char *state_offset,
		    const unsigned char bitstream_version);

int lzogeneric1x_1_compress(const unsigned char *in, size_t in_len,
		     unsigned char *out, size_t *out_len,
		     void *wrkmem, const unsigned char bitstream_version);

int lzo1x_1_compress(const unsigned char *in, size_t in_len,
		     unsigned char *out, size_t *out_len,
		     void *wrkmem);

int lzorle1x_1_compress(const unsigned char *in, size_t in_len,
		     unsigned char *out, size_t *out_len,
		     void *wrkmem);

#endif /* LZO1X_COMPRESS_H */