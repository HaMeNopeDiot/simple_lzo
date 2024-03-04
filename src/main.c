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

void test()
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

int main(int argc, char *argv[])
{
    test();
    return 0; //simple_lzo(argc, argv);
}