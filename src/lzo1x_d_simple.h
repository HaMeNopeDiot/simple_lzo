#ifndef LZO1X_D_SIMPLE_H
#define LZO1X_D_SIMPLE_H

#include <stdint.h>
#include <stdio.h>

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
    uint32_t distance;
    uint32_t dist_next_inst;
} lzo1x_dins_t;


// 0 0 0 0 X X X X  (0..15)
// 0 0 0 1 H L L L  (16..31)
// 0 0 1 L L L L L  (32..63)
// 0 1 L D D D S S  (64..127)
// 1 L L D D D S S  (128..255)

typedef struct {
    uint8_t l;
    uint8_t d;
    uint8_t s;
    uint8_t h;
} lzo1x_ins_fb_t;

lzo1x_ins_fb_t decode_first_byte_3(uint8_t byte);
lzo1x_ins_fb_t decode_first_byte_2(uint8_t byte);
lzo1x_ins_fb_t decode_first_byte_1(uint8_t byte);

uint32_t calc_dist_next_inst(const uint8_t* ip);

lzo1x_dins_t decode_instr3b(uint8_t* instruction);
lzo1x_dins_t decode_instr2b(uint8_t* instruction);
lzo1x_dins_t decode_instr1b(uint8_t* instruction);

void lzo1x_2bins_print(const lzo1x_ins_fb_t obj);
void lzo1x_dins_print(const lzo1x_dins_t obj);


#endif /* LZO1X_D_SIMPLE_H */