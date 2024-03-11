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

typedef struct {
    uint8_t state;
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

typedef union
{
    uint8_t fb;
    lzo1x_5b_t fb5;
    lzo1x_6b_t fb6;
    lzo1x_7b_t fb7;
    lzo1x_8b_t fb8;
}lzo1x_fb_t;

/* END OF FIRST BYTE STRUCTURES */

/* NEW FUNCTIONS */

void lzo1x_decode_instruction(uint8_t *ip);


/* OLD FUNCTIONS */

lzo1x_begin_t decode_first_inst(const uint8_t* ip);

lzo1x_ins_fb_t decode_first_byte_3(uint8_t byte);
lzo1x_ins_fb_t decode_first_byte_2(uint8_t byte);
lzo1x_ins_fb_t decode_first_byte_1(uint8_t byte);

uint32_t calc_dist_next_inst(const uint8_t* ip);

lzo1x_dins_t decode_instr3b(uint8_t* instruction);
lzo1x_dins_t decode_instr2b(uint8_t* instruction);
lzo1x_dins_t decode_instr1b(uint8_t* instruction);

void lzo1x_2bins_print(const lzo1x_ins_fb_t obj);
void lzo1x_dins_print(const lzo1x_dins_t obj);

void j_inst(uint8_t* input, uint8_t* output, size_t dist);


uint32_t count_zero_bytes_4b(uint8_t *ip);
uint32_t count_zero_bytes_3b(uint8_t *ip);

#endif /* LZO1X_D_SIMPLE_H */