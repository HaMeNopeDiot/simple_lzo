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
    uint8_t ii_1[] = {0x84, 0x07};
    uint8_t ii_2[] = {0xD8, 0x28};
    uint8_t ii_3[] = {0x99, 0x72};
        // 0 1 L D D D S S
    uint8_t ii_4[] = {0x78, 0x39};
        // 0 0 1 L L L L L
    uint8_t ii_5[] = {0x33, 0x04, 0x00};
    uint8_t ii_6[] = {0x21, 0x05, 0x01};
    uint8_t ii_7[] = {0x2F, 0x17, 0x23};
    uint8_t ii_8[] = {0x20, 0xFF, 0x17, 0x23};
    uint8_t ii_9[] = {0x20, 0x00, 0x30, 0x10, 0x43};
    uint8_t ii_10[] = {0x20, 0x00, 0x00, 0x00, 0x43, 0x21, 0x54};
        // 0 0 0 1 H L L L
    uint8_t ii_11[] = {0x10, 0x01, 0x04, 0x00};
    uint8_t ii_12[] = {0x18, 0x20, 0x07, 0x01};
    uint8_t ii_13[] = {0x1D, 0x01, 0x00};
    uint8_t ii_14[] = {0x10, 0x00, 0x05, 0x10, 0x20};
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
    printf("11:"); lzo1x_decode_instr(ii_11, 0);
    printf("12:"); lzo1x_decode_instr(ii_12, 0);
    printf("13:"); lzo1x_decode_instr(ii_13, 0);
    printf("14:"); lzo1x_decode_instr(ii_14, 0);
}

int main(int argc, char *argv[])
{
    testbench_decode_instructions();
    return 0;//simple_lzo(argc, argv);
}