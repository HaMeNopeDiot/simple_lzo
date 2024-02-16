// SPDX-License-Identifier: GPL-2.0-only
/*
 *  LZO1X Decompressor from LZO
 *
 *  Copyright (C) 1996-2012 Markus F.X.J. Oberhumer <markus@oberhumer.com>
 *
 *  The full LZO package can be found at:
 *  http://www.oberhumer.com/opensource/lzo/
 *
 *  Changed for Linux kernel use by:
 *  Nitin Gupta <nitingupta910@gmail.com>
 *  Richard Purdie <rpurdie@openedhand.com>
 */

/*
#ifndef STATIC
#include <linux/module.h>
#include <linux/kernel.h>
#endif
#include <asm/unaligned.h>
#include <linux/lzo.h>
#include "lzodefs.h"
*/

#ifndef LZO1X_DECOMPRESS_SAFE_H
#define LZO1X_DECOMPRESS_SAFE_H

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "lzo.h"

#define HAVE_IP(x)      ((size_t)(ip_end - ip) >= (size_t)(x))
#define HAVE_OP(x)      ((size_t)(op_end - op) >= (size_t)(x))
#define NEED_IP(x)      if (!HAVE_IP(x)) goto input_overrun
#define NEED_OP(x)      if (!HAVE_OP(x)) goto output_overrun
#define TEST_LB(m_pos)  if ((m_pos) < out) goto lookbehind_overrun

/* This MAX_255_COUNT is the maximum number of times we can add 255 to a base
 * count without overflowing an integer. The multiply will overflow when
 * multiplying 255 by more than MAXINT/255. The sum will overflow earlier
 * depending on the base count. Since the base count is taken from a u8
 * and a few bits, it is safe to assume that it will always be lower than
 * or equal to 2*255, thus we can always prevent any overflow by accepting
 * two less 255 steps. See Documentation/staging/lzo.rst for more information.
 */
#define MAX_255_COUNT      ((((size_t)~0) / 255) - 2)

// Void instances
#include "lzo.h"
#include "lzodefs.h"

int lzo1x_decompress_safe(const unsigned char *in, size_t in_len,
			  unsigned char *out, size_t *out_len);

#endif /* LZO1X_DECOMPRESS_SAFE_H */