#include "lzo1x_d_simple.h"

/* NEW FUNCTIONS */

static uint32_t lzo1x_cnt_length(uint8_t** ip, uint32_t length, uint16_t offset)
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

static lzo1x_dins_t lzo1x_dins_init()
{
    lzo1x_dins_t object;
    object.state      = 0;
    object.dist       = 0;
    object.len_instr  = 0;
    object.length     = 0;
    object.type_instr = 0;
    return object;
}


static uint8_t lzo1x_get_type_instr(lzo1x_fb_t first_b)
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
static lzo1x_dins_t lzo1x_fill_result_8(uint8_t* ip, lzo1x_fb_t first_b)
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
static lzo1x_dins_t lzo1x_fill_result_7(uint8_t* ip, lzo1x_fb_t first_b)
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
static lzo1x_dins_t lzo1x_fill_result_6(uint8_t* ip, lzo1x_fb_t first_b)
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
static lzo1x_dins_t lzo1x_fill_result_5(uint8_t* ip, lzo1x_fb_t first_b)
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
static lzo1x_dins_t lzo1x_fill_result_4(uint8_t* ip, lzo1x_fb_t first_b, uint32_t prev_stats)
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

static lzo1x_dins_t lzo1x_get_data_ins(uint8_t *ip, lzo1x_fb_t first_b, uint8_t type_of_instruction, uint32_t prev_state)
{
    lzo1x_dins_t result;
    result.dist = 0;
    result.length = 0;
    result.state = 0;
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
    result.type_instr = type_of_instruction;
    return result;
}

lzo1x_dins_t lzo1x_decode_instr(uint8_t *ip, uint32_t prev_state)
{
    lzo1x_fb_t first_b;
    first_b.fb = ip[0];
    // Get type of instruction
    uint8_t type_of_instruction = lzo1x_get_type_instr(first_b);
    // Get data from data blocks in instruction
    lzo1x_dins_t data = lzo1x_get_data_ins(ip, first_b, type_of_instruction, prev_state);
    return data;
}

static void lzo1x_cpy_from_input_p(uint8_t **ip, uint8_t **op, uint32_t state)
{
    while(state--) {
        *((*op)++) = *((*ip)++);
    }
}

static void lzo1x_cpy_from_dict(uint8_t **op, uint8_t **dp, uint32_t length)
{
    while(length--) {
        *((*op)++) = *((*dp)++);
    }
}

static int lzo1x_cmp_input_ptrs(const uint8_t *ip, const uint8_t *ip_end)
{
    return  ip_end == ip ? LZO_E_OK: 
            ip_end >  ip ? LZO_E_INPUT_NOT_CONSUMED: LZO_E_INPUT_OVERRUN;
}

static int lzo1x_cmp_output_ptrs(const uint8_t *op, const uint8_t *op_end)
{
    return op > op_end? LZO_E_OUTPUT_OVERRUN: LZO_E_OK;
}

int lzo1x_decompress_simple(uint8_t *in, size_t input_size, uint8_t *out, size_t *output_size)
{
    /* Start */
    uint8_t *ip = in;   // Pointer-start to input stream
    uint8_t *op = out;  // Pointer-start to output stream

    uint8_t *in_end = in + input_size;    // end of input stream
    uint8_t *out_end = op + *output_size; // maximum possible position of op
    /* Check minimal size of input stream */
    if(input_size < 3) { 
        return LZO_E_INPUT_OVERRUN; // compressed file can't have size less than 3 bytes
    }
    /* Decode first byte instruction */
    lzo1x_dins_t tmp_instr = lzo1x_dins_init(); // All fields this struct be 0.
    if(*ip > 16) {
        if(*ip == 17) {
            ip += 2; // Skip 2 bytes of bitstream version
        } else {
            tmp_instr.state = *ip - 17; // get count of copy symbols from input stream
            ip++; // Move pointer one byte forward
            lzo1x_cpy_from_input_p(&ip, &op, tmp_instr.state); // Copy bytes from input buffer to position op in output buffer. Both pointers must be go forward.
        }
    }
    /* Main part */
    bool stop = false;
    uint32_t prev_state = tmp_instr.state; // If first byte was a instruction, that means state can be value no 0.
    while(!stop) {
        /* Decode instruction */
        lzo1x_dins_t tmp_instr = lzo1x_decode_instr(ip, prev_state);
        ip += tmp_instr.len_instr; // Move pointer after instruction
        /* Checking stop flag */
        if((tmp_instr.type_instr == LZO1X_FB_5_T) && (tmp_instr.dist == KB16)) { // this is conditions is a stop signal to algorithm. If algorithm see this, that means compressed input stream is ended.
            stop = true;
        } else {
            /* Execute instruction */
            uint8_t *m_pos = op - tmp_instr.dist; // Pointer set to place, where from output stream (dictionary) need to copy. 
            lzo1x_cpy_from_dict(&op, &m_pos, tmp_instr.length); // Copy bytes from output buffer from m_pos to position op in output buffer. Both pointers must be go forward.
            lzo1x_cpy_from_input_p(&ip, &op, tmp_instr.state);  // Copy bytes from input buffer to position op in output buffer. Both pointers must be go forward.
            /* Check if output pointer don't go through out_end*/
            int status_output_ptr = lzo1x_cmp_output_ptrs(op, out_end); 
            if(status_output_ptr != LZO_E_OK) {
                return status_output_ptr;           // If something go wrong return with status error. In this case error is related to the output buffer
            }
        }
    }
    *output_size = op - out; // This is final size of output buffer
    return lzo1x_cmp_input_ptrs(ip, in_end); 
    /*
    * Last, need to check positions of ip(input pointer) and in_end(end of input stream). If ip != in_end, that means, something get wrong.
    * We have two scenarios: 
    *      1. Input data is not fully consumed or not consumed
    *      2. Input data overan was happen.
    */
}