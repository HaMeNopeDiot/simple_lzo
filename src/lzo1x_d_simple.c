#include "lzo1x_d_simple.h"

/* NEW FUNCTIONS */

uint32_t lzo1x_cnt_length(uint8_t** ip, uint32_t length, uint16_t offset)
{
    uint32_t tmp_length = length;
    uint8_t* tmp_p = *(ip) + 1; // set pointer after first byte instruction.
    if(tmp_length == 0) {
        while(*tmp_p == 0) { // if next byte is zero skip byte.
            tmp_p++; 
        }        
        uint32_t cnt_zeroes = (tmp_p - *ip) - 1; // distance between pointers is a count of bytes.
        tmp_length = offset + 255 * cnt_zeroes + *tmp_p;
        *(ip) = tmp_p;
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

// 128..255
lzo1x_dins_t lzo1x_fill_result_8(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    result.state = first_b.fb8.s;
    result.length = first_b.fb8.l + 5;
    result.dist = first_b.fb8.d + 1 + (ip[1] << 3);
    result.len_instr = 2;
    //ip += 2;
    return result;
}

// 64..127
lzo1x_dins_t lzo1x_fill_result_7(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    result.state = first_b.fb7.s;
    result.length = first_b.fb7.l + 3;
    result.dist = first_b.fb7.d + 1 + (ip[1] << 3);
    result.len_instr = 2;
    //ip += 2;
    return result;
}

// 32..63
lzo1x_dins_t lzo1x_fill_result_6(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    uint8_t *tmp_ip = ip;
    result.length = 2 + lzo1x_cnt_length(&ip, first_b.fb6.l, 31);
    uint16_t le16 = ((*(ip + 2) << NEXT_BYTE_OFFSET) + *(ip + 1));
    result.dist = (le16 >> 2) + 1;
    result.state = le16 & BYTE_STATE_MASK;
    result.len_instr = 3 + (uint32_t)(ip - tmp_ip);
    //ip += 3;
    return result;
}

// 16..31
lzo1x_dins_t lzo1x_fill_result_5(uint8_t* ip, lzo1x_fb_t first_b)
{
    lzo1x_dins_t result;
    uint8_t *tmp_ip = ip;
    result.length = 2 + lzo1x_cnt_length(&ip, first_b.fb5.l, 7);
    uint16_t le16 = ((*(ip + 2) << NEXT_BYTE_OFFSET) + *(ip + 1));
    result.state = le16 & BYTE_STATE_MASK;
    result.dist = KB16 + (first_b.fb5.h << 14) + (le16 >> 2);
    result.len_instr = 3 + (uint32_t)(ip - tmp_ip);
    //ip += 3;
    return result;
}

// 0..15
lzo1x_dins_t lzo1x_fill_result_4(uint8_t* ip, lzo1x_fb_t first_b, uint32_t prev_stats)
{
    lzo1x_dins_t result;
    if(prev_stats == 0) {
        // if prev isntruction copy 0 literals from input buffer
        uint8_t* tmp_ip = ip;
        result.state = 3 + lzo1x_cnt_length(&ip, first_b.fb4z.l, 15);
        result.length = 0;
        result.dist = 0;
        result.len_instr = 1 + (ip - tmp_ip);
        ip += 1;
    } else { 
        uint16_t h = *(ip + 1);
        result.state = first_b.fb4s.s;
        if(prev_stats >= 4) {
            // if prev instructions copy 4 or more literals
            result.dist = (h << 2) + first_b.fb4s.d + 2049;
            result.length = 3;
            result.len_instr = 2;
        } else {
            // if prev instruction copy between 1..3 literals from input buffer
            result.dist = (h << 2) + first_b.fb4s.d + 1;
            result.length = 2;
            result.len_instr = 2;
        }
        ip += 2;
    }
    return result;
}

lzo1x_dins_t lzo1x_get_data_ins(uint8_t *ip, lzo1x_fb_t first_b, uint8_t type_of_instruction, uint32_t prev_state)
{
    lzo1x_dins_t result;
    result.dist = 0;
    result.length = 0;
    result.state = 0;
    result.type_instr = type_of_instruction;
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
        /* 0 0 0 0 X X X X */
        result = lzo1x_fill_result_4(ip, first_b, prev_state);
        break;
    default:
        printf("Unexpected instruction!\n");
        exit(1);
        break;
    }
    return result;
}

lzo1x_dins_t lzo1x_decode_instr(uint8_t *ip, uint32_t prev_state)
{
    lzo1x_fb_t first_b;
    first_b.fb = ip[0];
    printf("tb: 0x%-2x -> ",ip[0]);
    // Get type of instruction
    uint8_t type_of_instruction = lzo1x_get_type_instr(first_b);
    // Get data from data blocks in instruction
    lzo1x_dins_t data = lzo1x_get_data_ins(ip, first_b, type_of_instruction, prev_state);
    printf("type: %-2d dist: %-10d state: %-7d length: %-10d, len_inst: %-5d\n", type_of_instruction, data.dist, data.state, data.length, data.len_instr);
    return data;
}


void lzo1x_decode(uint8_t *in, size_t input_size, uint8_t *out, size_t output_size)
{
    // Start
    uint8_t *ip = in;
    uint8_t *op = out;
    // Decode first byte instruction

    // Main part
    bool stop = false;
    uint32_t prev_state = 0;
    while(!stop) {
        // Decode instruction
        lzo1x_dins_t tmp_instr = lzo1x_decode_instr(ip, prev_state);
        ip += tmp_instr.len_instr;
        // Checking stop flag
        if((tmp_instr.type_instr == LZO1X_FB_5_T) && (tmp_instr.dist == KB16)) {
            stop = true;
        } else {
            /* Execute instruction */
            uint8_t *m_pos = op - tmp_instr.dist;
            uint32_t length = tmp_instr.length;
            while(length) {
                *(op++) = *(m_pos++);
            }
            uint32_t state = tmp_instr.state;
            while(state) {
                *(op++) = *(ip++);
            }

        }
    }

}