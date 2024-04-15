#include "lzo1x_d_simple.h"

/* NEW FUNCTIONS */

uint8_t lzo1x_get_type_instruction(lzo1x_fb_t first_b)
{
    uint8_t type_of_instruction;
    if(first_b.fb8.prefix == LZO1X_PREFIX_TAG) {
        type_of_instruction = LZO1X_FB_8_T;
    } else {
        if(first_b.fb7.prefix == LZO1X_PREFIX_TAG) {
            type_of_instruction = LZO1X_FB_7_T;
        } else {
            if(first_b.fb6.prefix == LZO1X_PREFIX_TAG) {
                type_of_instruction = LZO1X_FB_6_T;
            } else {
                if(first_b.fb5.prefix == LZO1X_PREFIX_TAG) {
                    type_of_instruction = LZO1X_FB_5_T;
                } else {
                    type_of_instruction = LZO1X_FB_4_T;
                }
            }
        }
    }
    return type_of_instruction;
}

lzo1x_dins_t lzo1x_fill_result_8(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    result.state = first_b.fb8.s;
    result.length = first_b.fb8.l + 5;
    result.dist = first_b.fb8.d + 1 + (ip[1] << 3);
    ip += 2;
    return result;
}

lzo1x_dins_t lzo1x_fill_result_7(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    result.state = first_b.fb7.s;
    result.length = first_b.fb7.l + 3;
    result.dist = first_b.fb7.d + 1 + (ip[1] << 3);
    ip += 2;
    return result;
}

lzo1x_dins_t lzo1x_fill_result_6(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    result.length = first_b.fb6.l + 2;
    uint16_t le16 = ((*(ip + 1) << NEXT_BYTE_OFFSET) + *(ip + 2));
    result.dist = (le16 >> 2) + 1;
    result.state = le16 & BYTE_STATE_MASK;
    ip += 3;
    return result;
}

lzo1x_dins_t lzo1x_fill_result_5(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    uint16_t le16 = ((*(ip + 1) << NEXT_BYTE_OFFSET) + *(ip + 2));
    result.length = first_b.fb5.l;
    result.state = le16 & BYTE_STATE_MASK;
    result.dist = KB16 + (first_b.fb5.h << 14) + (le16 >> 2);
    ip += 3;
    return result;
}

lzo1x_dins_t lzo1x_get_data_ins(uint8_t *ip, lzo1x_fb_t first_b, uint8_t type_of_instruction)
{
    lzo1x_dins_t result;
    result.dist = 0;
    result.length = 0;
    result.state = 0;
    result.dist_next = 0;
    switch (type_of_instruction) {
    case LZO1X_FB_8_T: //128..255
        /* 1 L L D D D S  */
        result = lzo1x_fill_result_8(ip, first_b);
        break;
    case LZO1X_FB_7_T: //64..127
        /* 0 1 L D D D S S */
        result = lzo1x_fill_result_7(ip, first_b);
        break;
    case LZO1X_FB_6_T: //32..63
        /* 0 0 1 L L L L L */
        result = lzo1x_fill_result_6(ip, first_b);
        break;
    case LZO1X_FB_5_T: //16..31
        /* 0 0 0 1 H L L L */
        result = lzo1x_fill_result_5(ip, first_b);
        break;
    case LZO1X_FB_4_T: //0..15
        /* code */
        break;
    default:
        printf("Unexpected instruction!\n");
        exit(1);
        break;
    }
    return result;
}

void lzo1x_decode_instruction(uint8_t *ip)
{
    lzo1x_fb_t first_b;
    first_b.fb = ip[0];
    printf("tb: 0x%x -> ",ip[0]);
    // Get type of instruction
    uint8_t type_of_instruction = lzo1x_get_type_instruction(first_b);
    // Get data from data blocks in instruction
    lzo1x_dins_t data = lzo1x_get_data_ins(ip, first_b, type_of_instruction);
    printf("type: %d, dist: %d, state: %d, length: %d\n", type_of_instruction, data.dist, data.state, data.length);
}









/* OLD FUNCTIONS */

lzo1x_begin_t decode_first_inst(const uint8_t* ip) // still don't work on not null bitstream
{
    lzo1x_begin_t instruction;
    instruction.bitstream_version = *ip;
    instruction.dist_value = *(ip+1);
    return instruction;
}

void j_inst(uint8_t* input, uint8_t* output, size_t dist) //jump to next instruction
{
    while(dist--) {
        *output++ = *input++;
    }
}

lzo1x_ins_fb_t decode_first_byte_4(uint8_t byte) // decoding first byte of instruction in range 16..31
{
    lzo1x_ins_fb_t result; // 0 0 0 1 H L L L (16..31)
    result.d = 0;
    result.h = (byte & 0b00001000) >> 3;
    result.l = (byte & 0b00000111);
    result.s = 0;

    return result;
}

lzo1x_ins_fb_t decode_first_byte_3(uint8_t byte) // decoding first byte of instruction in range 32..63
{
    lzo1x_ins_fb_t result; // 0 0 1 L L L L L (32..63)
    result.s = 0;
    result.d = 0;
    result.h = 0;
    result.l = (byte & 0b00011111);
    return result;
}

lzo1x_ins_fb_t decode_first_byte_2(uint8_t byte) // decoding first byte of instruction in range 64..127
{
    lzo1x_ins_fb_t result; // 0 1 L D D D S S (64..127)
    result.h = 0;
    result.s = byte & 0b00000011;
    result.d = (byte & 0b00011100) >> 2;
    result.l = (byte & 0b00100000) >> 5;
    return result;
}

lzo1x_ins_fb_t decode_first_byte_1(uint8_t byte) // decoding first byte of instruction in range 128..255
{
    lzo1x_ins_fb_t result; // 1 L L D D D S S (128..255)
    result.h = 0;
    result.s = byte & 0b00000011;
    result.d = (byte & 0b00011100) >> 2;
    result.l = (byte & 0b01100000) >> 5;
    return result;
}

uint32_t calc_dist_next_inst(const uint8_t* ip)
{
    uint32_t dist_next = 3;
    if(*ip++ == 0) {
        dist_next += 15;
        uint32_t offset = 0;
        while(*ip == 0) {
            offset++;
            ip++;
        }
        offset = (offset << 8) - offset;
        dist_next += offset + *ip++;
    }
    return dist_next;
}

uint32_t count_zero_bytes(uint8_t *ip, uint8_t num)
{
    uint32_t offset = 0;
    while(*ip == 0) {
        offset++;
        ip++;
    }
    return (num + (offset << 8) + offset + *ip++);
}

uint32_t count_zero_bytes_4b(uint8_t *ip)
{
    return count_zero_bytes(ip, 7);
}

uint32_t count_zero_bytes_3b(uint8_t *ip)
{
    return count_zero_bytes(ip, 31);
}

lzo1x_dins_t decode_instr3b(uint8_t *instruction) // 32..63
{
    lzo1x_dins_t result;
    uint8_t *ip = instruction;
    lzo1x_ins_fb_t values = decode_first_byte_3(*ip++);
    // ip on next byte
    if(values.l == 0) {
        values.l = count_zero_bytes_3b(ip); // 31 + (offset << 8) + offset + *ip++;
    }
    //reading LE16
    uint16_t le_data = ((*ip) << 8) + *(ip+1);
    ip += 2;
    values.s = le_data & 0b11;
    values.d = le_data >> 2;

    //write results
    result.state = values.s;
    result.length = 2 + values.l;
    result.dist = 1 + values.d;

    //calc next instruction
    result.dist_next = calc_dist_next_inst(ip);

    return result; 
}

lzo1x_dins_t decode_instr2b(uint8_t *instruction) // 64..127
{
    lzo1x_dins_t result;
    const uint8_t *ip = instruction;

    // read first byte
    lzo1x_ins_fb_t values = decode_first_byte_2(*ip++);

    // read next byte
    values.h = *ip++;

    // write results
    result.state = values.s;
    result.length = 3 + values.l;
    result.dist = (values.h << 3) + values.d + 1;

    // calc next instruction
    result.dist_next = calc_dist_next_inst(ip);

    return result;
}

lzo1x_dins_t decode_instr1b(uint8_t* instruction) // 128..255
{
    lzo1x_dins_t result;
    const uint8_t *ip = instruction;

    // read first byte
    lzo1x_ins_fb_t values = decode_first_byte_1(*ip++);
    // read second byte
    uint8_t h = *ip++;

    // write results
    result.state = values.s;
    result.length = 5 + values.l;
    result.dist = (h << 3) + values.d + 1;

    // calc next instruction
    result.dist_next = calc_dist_next_inst(ip);

    return result;
}

void lzo1x_2bins_print(const lzo1x_ins_fb_t obj)
{
    printf("S = %d; D = %d; L = %d\n", obj.s, obj.d, obj.l);
}

void lzo1x_dins_print(const lzo1x_dins_t obj)
{
    printf("state: %d; length: %d; dist: %d; dist_next: %d\n", \
    obj.state, obj.length, obj.dist, obj.dist_next);
}