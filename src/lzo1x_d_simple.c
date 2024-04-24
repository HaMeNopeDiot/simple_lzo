#include "lzo1x_d_simple.h"

/* NEW FUNCTIONS */

uint32_t lzo1x_cnt_length(uint8_t* ip, uint32_t length)
{
    uint32_t tmp_length = length;
    uint8_t* tmp_p = ip + 1; // set pointer after first byte instruction.
    if(tmp_length == 0) {
        while(*tmp_p == 0) { // if next byte is zero skip byte.
            tmp_p++; 
        }        
        uint32_t cnt_zeroes = tmp_p - ip; // distance between pointers is a count of bytes.
        tmp_length = 15 + 255 * cnt_zeroes + *tmp_p;
    }
    return tmp_length;
}



uint8_t lzo1x_get_type_instr(lzo1x_fb_t first_b)
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

lzo1x_dins_t lzo1x_fill_result_4(uint8_t* ip, lzo1x_fb_t first_b, uint32_t prev_stats)
{
    lzo1x_dins_t result;
    if(prev_stats == 0) {
        // if prev isntruction copy 0 literals from input buffer
        result.state = 3 + lzo1x_cnt_length(ip, first_b.fb4z.l);
        result.length = 0;
        result.dist = 0;
        // result.dist_next = 0;
    } else { 
        uint16_t h = *(ip + 1);
        result.state = first_b.fb4s.s;
        if(prev_stats >= 4) {
            // if prev instructions copy 4 or more literals
            result.dist = (h << 2) + first_b.fb4s.d + 2049;
            result.length = 3;
        } else {
            // if prev instruction copy between 1..3 literals from input buffer
            result.dist = (h << 2) + first_b.fb4s.d + 1;
            result.length = 2;
        }
    }
    ip += 1;
    return result;
}

lzo1x_dins_t lzo1x_get_data_ins(uint8_t *ip, lzo1x_fb_t first_b, uint8_t type_of_instruction, uint32_t prev_state)
{
    lzo1x_dins_t result;
    result.dist = 0;
    result.length = 0;
    result.state = 0;
    // result.dist_next = 0;
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

void lzo1x_decode_instr(uint8_t *ip, uint32_t prev_state)
{
    lzo1x_fb_t first_b;
    first_b.fb = ip[0];
    printf("tb: 0x%x -> ",ip[0]);
    // Get type of instruction
    uint8_t type_of_instruction = lzo1x_get_type_instr(first_b);
    // Get data from data blocks in instruction
    lzo1x_dins_t data = lzo1x_get_data_ins(ip, first_b, type_of_instruction, prev_state);
    printf("type: %d, dist: %d, state: %d, length: %d\n", type_of_instruction, data.dist, data.state, data.length);
}
