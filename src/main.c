#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lzo_shell.h"

bool Verbose = false;

char* get_status_prs_args(int status)
{
    char* status_str;
    switch(status) {
        case ERRR_STATUS     :
            status_str = strdup(ERRR_STR_STATUS);
            break;
        case HELP_STATUS     :
            status_str = strdup(HELP_STR_STATUS);
            break;
        case TEST_STATUS     :
            status_str = strdup(TEST_STR_STATUS);
            break;
        case COMP_STATUS     :
            status_str = strdup(COMP_STR_STATUS);
            break;
        case DCMP_STATUS     :
            status_str = strdup(DCMP_STR_STATUS);
            break;
        case DCMP_STATUS_SMP :
            status_str = strdup(DCMP_STR_STATUS_SMP);
            break;
        default:
            status_str = strdup("Unknown status");
            break;
    }
    return status_str;
}

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

    verbose("INIT\nSource: %s\nOut: %s\nStatus: \"%s\" (%d)\n", \
    init_obj->input_file, init_obj->output_file,                \
    get_status_prs_args(init_obj->status), init_obj->status);

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
    case DCMP_STATUS_SMP:
        status_prog = lzo_simple_decode(init_obj->input_file, init_obj->output_file);
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
        // 0 0 0 0 X X X X
            // 0 0 0 0 L L L L (== 0)
    uint8_t ii_15[] = {0x07};
    uint8_t ii_16[] = {0x00, 0xAB};
    uint8_t ii_17[] = {0x00, 0x00, 0xDE};
            // 0 0 0 0 D D S S (!= 0)
    uint8_t ii_18[] = {0x07, 0x02};
    uint8_t ii_19[] = {0x02, 0x03};
    uint8_t ii_20[] = {0x08, 0x08};
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
    printf("15:"); lzo1x_decode_instr(ii_15, 0);
    printf("16:"); lzo1x_decode_instr(ii_16, 0);
    printf("17:"); lzo1x_decode_instr(ii_17, 0);
    printf("18:"); lzo1x_decode_instr(ii_18, 1);
    printf("19:"); lzo1x_decode_instr(ii_19, 2);
    printf("20:"); lzo1x_decode_instr(ii_20, 3);
    printf("18:"); lzo1x_decode_instr(ii_18, 4);
    printf("19:"); lzo1x_decode_instr(ii_19, 4);
    printf("20:"); lzo1x_decode_instr(ii_20, 4);
}

void testbench_lzo_decoding_stream()
{
    uint8_t input_stream[] = {0x06, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x50, 0x00, 0x11, 0x00, 0x00};
    //printf("dec: %ld\n", sizeof(input_stream));
    uint8_t *output_stream = (uint8_t*)calloc(sizeof(input_stream) * 2, sizeof(uint8_t));
    size_t inpt_len = sizeof(input_stream);
    size_t outp_len = inpt_len * 2;
    lzo1x_decompress_simple(input_stream, inpt_len, output_stream, outp_len);

    printf("Input:");
    for(size_t i = 0; i < inpt_len; i++) {
        printf("0x%-2x ", input_stream[i]);
    }
    printf("\n");
    printf("Output:");
    for(size_t i = 0; i < outp_len; i++) {
        printf("0x%-2x ", output_stream[i]);
    }
    printf("\n");

    free(output_stream);
}

int main(int argc, char *argv[])
{
    //  testbench_decode_instructions();
    // testbench_lzo_decoding_stream();
    simple_lzo(argc, argv);
    return 0;
}