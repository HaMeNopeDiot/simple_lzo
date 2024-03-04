#include "lzo1x_d_simple.h"

lzo1x_ins_fb_t decode_first_byte_4(uint8_t byte) 
{
    lzo1x_ins_fb_t result;
    result.d = 0;
    result.h = (byte & 0b00001000) >> 3;
    result.l = (byte & 0b00000111);
    result.s = 0;

    return result;
}

lzo1x_ins_fb_t decode_first_byte_3(uint8_t byte)
{
    lzo1x_ins_fb_t result;
    result.s = 0;
    result.d = 0;
    result.h = 0;
    result.l = (byte & 0b00011111);
    return result;
}

lzo1x_ins_fb_t decode_first_byte_2(uint8_t byte) 
{
    lzo1x_ins_fb_t result;
    result.h = 0;
    result.s = byte & 0b00000011;
    result.d = (byte & 0b00011100) >> 2;
    result.l = (byte & 0b00100000) >> 5;
    return result;
}

lzo1x_ins_fb_t decode_first_byte_1(uint8_t byte)
{
    lzo1x_ins_fb_t result;
    result.h = 0;
    result.s = byte & 0b00000011;
    result.d = (byte & 0b00011100) >> 2;
    result.l = (byte & 0b01100000) >> 5;
    return result;
}

uint32_t calc_dist_next_inst(const uint8_t* ip)
{
    uint32_t dist_next_inst = 3;
    if(*ip++ == 0) {
        dist_next_inst += 15;
        uint32_t offset = 0;
        while(*ip == 0) {
            offset++;
            ip++;
        }
        offset = (offset << 8) - offset;
        dist_next_inst += offset + *ip++;
    }
    return dist_next_inst;
}

lzo1x_dins_t decode_instr3b(uint8_t *instruction)
{
    lzo1x_dins_t result;
    const uint8_t *ip = instruction;
    lzo1x_ins_fb_t values = decode_first_byte_3(*ip++);
    if(values.l == 0) {
        uint32_t offset = 0;
        while(*ip == 0) {
            offset++;
            ip++;
        }
        values.l = 31 + (offset << 8) + offset;
    }
    uint16_t le_data = ((*ip) << 8) + *(ip+1);
    ip += 2;
    values.s = le_data & 0b11;
    values.d = le_data >> 2;

    result.state = values.s;
    result.length = 2 + values.l;
    result.distance = 1 + values.d;
    result.dist_next_inst = calc_dist_next_inst(ip);

    return result; 
}

lzo1x_dins_t decode_instr2b(uint8_t *instruction) // 64
{
    lzo1x_dins_t result;
    const uint8_t *ip = instruction;
    lzo1x_ins_fb_t values = decode_first_byte_2(*ip++);
    values.h = *ip++;

    result.state = values.s;
    result.length = 3 + values.l;
    result.distance = (values.h << 3) + values.d + 1;

    result.dist_next_inst = calc_dist_next_inst(ip);

    return result;
}

lzo1x_dins_t decode_instr1b(uint8_t* instruction)
{
    lzo1x_dins_t result;
    const uint8_t *ip = instruction;
    lzo1x_ins_fb_t values = decode_first_byte_1(*ip++);
    uint8_t h = *ip++;

    result.state = values.s;
    result.length = 5 + values.l;
    result.distance = (h << 3) + values.d + 1;
    result.dist_next_inst = calc_dist_next_inst(ip);

    return result;
}

void lzo1x_2bins_print(const lzo1x_ins_fb_t obj)
{
    printf("S = %d; D = %d; L = %d\n", obj.s, obj.d, obj.l);
}

void lzo1x_dins_print(const lzo1x_dins_t obj)
{
    printf("state: %d; length: %d; distance: %d; dist_next_inst: %d\n", \
    obj.state, obj.length, obj.distance, obj.dist_next_inst);
}