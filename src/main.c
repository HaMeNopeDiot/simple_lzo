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

void test() // test decoding function
{
    //return simple_lzo(argc, argv);
    uint8_t* input_data = (uint8_t*)malloc(sizeof(uint8_t) * 5);
    input_data[0] = 0x6C;
    input_data[1] = 0x01;
    input_data[2] = 0x00;
    input_data[3] = 0x10;
    input_data[4] = 0x45;
    lzo1x_dins_t res = decode_instr2b(input_data);
    lzo1x_dins_print(res);
    free(input_data);
}

void test2() // test jump after first instruction
{
    uint8_t* input_data = (uint8_t*)strdup("00what a awesome day today! Let's go walk in park!");
    //input_data[0] = (char)0x00;
    //input_data[1] = (char)0x10;
    *input_data = 0x00;
    *(input_data+1) = 0x10;
    //printf("text: %s\n", (char*)input_data);
    printf("data:\n");
    for(size_t i = 0; i < 50; i++) {
        printf("%x ", (uint8_t)input_data[i]);
    }
    printf("\n");
    
    lzo1x_begin_t beg_data = decode_first_inst(input_data);
    uint8_t* output_data = (uint8_t*)malloc(sizeof(uint8_t) * 32);
    
    uint8_t* op = output_data;
    uint8_t* ip = input_data;
    ip += 2;

    printf("distance: %d\n", beg_data.dist_value);
    j_inst(ip, op, beg_data.dist_value);

    //printf("text: %s\n", (char*)output_data);
    printf("data:\n");
    for(size_t i = 0; i < 32; i++) {
        printf("%x ", (uint8_t)output_data[i]);
    }
    printf("\n");

    
    free(output_data);
    free(input_data);
}

void test3()
{
    uint8_t* ii_1 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_1[0] = 0x84;
    ii_1[1] = 0x07;
    uint8_t* ii_2 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_2[0] = 0xD8;
    ii_2[1] = 0x28;
    uint8_t* ii_3 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_3[0] = 0x99;
    ii_3[1] = 0x72;
    uint8_t* ii_4 = (uint8_t*)malloc(sizeof(uint8_t) * 2);
    ii_4[0] = 0x78;
    ii_4[1] = 0x39;

    lzo1x_decode_instr(ii_1, 0);
    lzo1x_decode_instr(ii_2, 0);
    lzo1x_decode_instr(ii_3, 0);
    lzo1x_decode_instr(ii_4, 0);

    free(ii_1);
    free(ii_2);
    free(ii_3);
    free(ii_4);
    // lzo1x_decode_instr
}

int main(int argc, char *argv[])
{
    test3();
    return 0;//simple_lzo(argc, argv);
}