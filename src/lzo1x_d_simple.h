#ifndef LZO1X_D_SIMPLE_H
#define LZO1X_D_SIMPLE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define LZO1X_FB_8_T 8
#define LZO1X_FB_7_T 7
#define LZO1X_FB_6_T 6
#define LZO1X_FB_5_T 5
#define LZO1X_FB_4_T 4
#define LZO1X_PREFIX_TAG 1


#define NEXT_BYTE_OFFSET 8
#define BYTE_STATE_MASK 3

#define KB16 16384

typedef struct {
    uint8_t bitstream_version;
    uint8_t dist_value;
}lzo1x_begin_t;

typedef struct {
    uint8_t header;
    uint8_t *data;
    uint8_t *dist_n;
}lzo1x_inst_t;


/* 
    Parameters readed from instruction. Each instruction may contain values like state, lenght, dist, dist_next
    state. This parameter have two definitions:
        - 1. Status of current instruction. This need only for some next instructions (from rande [0..15]).
             Instruction read prevent state of instructions and interprets instructions differently depending
             on the previous state
        - 2. Count of copyed instructions from input buffer.
    length. Count of bytes need to copy from dictinary (output buffer)
    dist. Distance in bytes, where need to move output pointer to take bytes.
    dist_next. Distance in bytes to next instruction in input buffer.
    
*/
typedef struct {
    uint32_t state;
    uint32_t length;
    uint32_t dist;
    uint32_t dist_next;
} lzo1x_dins_t;


// 0 0 0 0 X X X X  (0..15)
// 0 0 0 1 H L L L  (16..31)
// 0 0 1 L L L L L  (32..63)
// 0 1 L D D D S S  (64..127)
// 1 L L D D D S S  (128..255)

typedef struct {
    uint32_t l;
    uint8_t d;
    uint8_t s;
    uint8_t h;
} lzo1x_ins_fb_t;


/* FIRST BYTE STUCTURES */

#pragma pack(1)

typedef struct {
    unsigned s: 2;
    unsigned d: 3;
    unsigned l: 2;
    unsigned prefix: 1;
} lzo1x_8b_t;

typedef struct {
    unsigned s: 2;
    unsigned d: 3;
    unsigned l: 1;
    unsigned prefix: 2;
} lzo1x_7b_t;

typedef struct {
    unsigned l: 5;
    unsigned prefix: 3;
} lzo1x_6b_t;

typedef struct {
    unsigned l: 3;
    unsigned h: 1;
    unsigned prefix: 4;
} lzo1x_5b_t;

typedef struct {
    unsigned l: 4;
    unsigned prefix: 4;
} lzo1x_4b0_t;

typedef struct {
    unsigned s: 2;
    unsigned d: 2;
    unsigned prefix: 4;
} lzo1x_4bs_t;

typedef union
{
    uint8_t fb;
    // [0..15]
    lzo1x_4b0_t fb4z; // if state Zero      (Z)
    lzo1x_4bs_t fb4s; // if state not zero  (S)
    // [16..255]
    lzo1x_5b_t fb5; // [16..31]
    lzo1x_6b_t fb6; // [32..63]
    lzo1x_7b_t fb7; // [64..127]
    lzo1x_8b_t fb8; // [128..255]
}lzo1x_fb_t;

/* END OF FIRST BYTE STRUCTURES */

/* NEW FUNCTIONS */

void lzo1x_decode_instr(uint8_t *ip, uint32_t prev_state);



#endif /* LZO1X_D_SIMPLE_H */