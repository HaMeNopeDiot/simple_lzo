#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lzo_shell.h"

bool Verbose = false;

int simple_lzo(int argc, char* argv[])
{
    prs_args_t *init_obj = parse_args(argc, argv);
    setVerbose(init_obj->verbose);
    if(init_obj->input_file == NULL) {
        init_obj->input_file = strdup(DEFAULT_INPUT_FILE);
    }
    if(init_obj->output_file == NULL) {
        char* folder = get_folder(init_obj->input_file);
        if(init_obj->status == COMP_STATUS) {
            init_obj->output_file = get_full_path(DEFAULT_COMP_OUT, folder);
        } else {
            init_obj->output_file = get_full_path(DEFAULT_DECOMP_OUT, folder);
        }
        free(folder);
    }
    verbose("INIT\nSource: %s\nOut: %s\nStatus: %d\n", init_obj->input_file, init_obj->output_file, init_obj->status);
    int status_prog = 0;
    switch (init_obj->status)
    {
    case DCMP_STATUS:
        status_prog = lzo_decompress(init_obj->input_file, init_obj->output_file);
        break;
    case COMP_STATUS:
        status_prog = lzo_compress(init_obj->input_file, init_obj->output_file, init_obj->btstrm_ver);
        break;
    case TEST_STATUS:
        status_prog = lzo_test(init_obj->input_file, init_obj->output_file, init_obj->btstrm_ver);
        break;
    default:
        status_prog = 0;
        break;
    }
    prs_args_free(init_obj);
    return status_prog;
}

uint8_t* init_instruction(size_t size, uint8_t *instruction)
{
    uint8_t* tmp_intst = (uint8_t*)malloc(sizeof(uint8_t) * size);
    for(size_t i = 0; i < size; i++) {
        tmp_intst[i] = instruction[i];
    }
    return tmp_intst;
}

void testbench_decode_instructions()
{
    // INITIALIZATION
        // 1 L L D D D S S
    uint8_t* ii_1 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_1[0] = 0x84; // 1000 0100 - first byte
    ii_1[1] = 0x07; // 0000 0111 - HHHH HHHH
    uint8_t* ii_2 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_2[0] = 0xD8; // 1101 1000 - first byte
    ii_2[1] = 0x28; // 0010 1000 - HHHH HHHH
    uint8_t* ii_3 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_3[0] = 0x99; // 1001 1001 - first byte
    ii_3[1] = 0x72; // 0111 0010 - HHHH HHHH
        // 0 1 L D D D S S
    uint8_t* ii_4 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_4[0] = 0x78; // 0111 1000 - first byte
    ii_4[1] = 0x39; // 0011 1001 - HHHH HHHH
        // 0 0 1 L L L L L
    uint8_t* ii_5 = (uint8_t*)malloc(sizeof(uint8_t) * 3);
    ii_5[0] = 0x33; // 0011 0011 - first byte
    ii_5[1] = 0x04; // 0000 0100 - DDDD DDSS
    ii_5[2] = 0x00; // 0000 0000 - DDDD DDDD
    uint8_t* ii_6 = (uint8_t*)malloc(sizeof(uint8_t) * 3);
    ii_6[0] = 0x21; // 0010 0001 - first byte
    ii_6[1] = 0x05; // 0000 0101 - DDDD DDSS
    ii_6[2] = 0x01; // 0000 0001 - DDDD DDDD
    uint8_t* ii_7 = (uint8_t*)malloc(sizeof(uint8_t) * 3);
    ii_7[0] = 0x2F; // 0010 1111 - first byte
    ii_7[1] = 0x17; // 0001 0111 - DDDD DDSS
    ii_7[2] = 0x23; // 0010 0011 - DDDD DDDD
    uint8_t* ii_8 = (uint8_t*)malloc(sizeof(uint8_t) * 4);
    ii_8[0] = 0x20; // 0010 0000 - first byte
    ii_8[1] = 0xFF; // 1111 1111 - non zero byte
    ii_8[2] = 0x17; // 0001 0111 - DDDD DDSS
    ii_8[3] = 0x23; // 0010 0011 - DDDD DDDD
    uint8_t* ii_9 = (uint8_t*)malloc(sizeof(uint8_t) * 5);
    ii_9[0] = 0x20; // 0010 0000 - first byte
    ii_9[1] = 0x00; // 0000 0000 (oh yes?) 
    ii_9[2] = 0x30; // 0011 0000 - non zero byte
    ii_9[3] = 0x10; // 0001 0111 - DDDD DDSS
    ii_9[4] = 0x43; // 0010 0011 - DDDD DDDD
    uint8_t* ii_10 = (uint8_t*)malloc(sizeof(uint8_t) * 7);
    ii_10[0] = 0x20; // 0010 0000 - first byte
    ii_10[1] = 0x00; // 0000 0000 (oh yes?) 
    ii_10[2] = 0x00; // 0000 0000 (oh yes?) 
    ii_10[3] = 0x00; // 0000 0000 (oh yes?) 
    ii_10[4] = 0x43; // 0011 0000 - non zero byte
    ii_10[5] = 0x21; // 0001 0111 - DDDD DDSS
    ii_10[6] = 0x54; // 0010 0011 - DDDD DDDD
    // DECODING
    printf("01:"); lzo1x_decode_instr(ii_1, 0);
    printf("02:"); lzo1x_decode_instr(ii_2, 0);
    printf("03:"); lzo1x_decode_instr(ii_3, 0);
    printf("04:"); lzo1x_decode_instr(ii_4, 0);
    printf("05:"); lzo1x_decode_instr(ii_5, 0);
    printf("06:"); lzo1x_decode_instr(ii_6, 0);
    printf("07:"); lzo1x_decode_instr(ii_7, 0);
    printf("08:"); lzo1x_decode_instr(ii_8, 0);
    printf("09:"); lzo1x_decode_instr(ii_9, 0);
    printf("10:"); lzo1x_decode_instr(ii_10, 0);
    // FREE MEMORY
    free(ii_1);
    free(ii_2);
    free(ii_3);
    free(ii_4);
    free(ii_5);
    free(ii_6);
    free(ii_7);
    free(ii_8);
    free(ii_9);
    free(ii_10);
    // lzo1x_decode_instr
}

int main(int argc, char *argv[])
{
    testbench_decode_instructions();
    return 0;//simple_lzo(argc, argv);
}