#include "parse_args.h"

void show_help()
{
    printf(
        "Help note, how to use this prog\n"
        "   -h            help\n"
        "   -t            testing by compress and decompress\n"
        "   -d            decompress mode. If uncheck, programm will enable compress mode\n"
        "   -i            input file\n"
        "   -o            output file\n"
        "   -v            verbose\n"
        "   -b            bitstream\n"
    );
}

prs_args_t* prs_args_init()
{
    prs_args_t* obj = (prs_args_t*)malloc(sizeof(prs_args_t));
    obj->status = COMP_STATUS;
    obj->verbose = false;
    obj->input_file = NULL;
    obj->output_file = NULL;
    obj->btstrm_ver = 0;
    return obj;
}

void prs_args_free(prs_args_t* obj)
{
    if(obj->input_file != NULL) {
        free(obj->input_file);
    }
    if(obj->output_file != NULL) {
        free(obj->output_file);
    }
    free(obj);
}

prs_args_t* parse_args(int argc, char *argv[])
{
    int result_opt = 0;
    prs_args_t* result = prs_args_init();
    while((result_opt = getopt(argc, argv, "bhtdi:o:v")) != -1)
    {
        switch(result_opt) {
            case 'h': 
                show_help();
                result->status = HELP_STATUS;
                return result;
            break; // help
            case 't': 
                result->status = TEST_STATUS;
            break; // test
            case 'd': 
                result->status = DCMP_STATUS;
            break; // decompress
            case 'i': 
                result->input_file = strdup(optarg);
            break; // input
            case 'o':
                result->output_file = strdup(optarg);
            break; // output
            case 'v':
                result->verbose = true;
            break; // verbose
            case 'b':
                result->btstrm_ver = 1;
            break;
            default:
                result->status = ERRR_STATUS;
                return result;
            break;
        }
    }
    return result;
}