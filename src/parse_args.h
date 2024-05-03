#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define ERRR_STATUS     -1
#define HELP_STATUS     0
#define TEST_STATUS     1
#define COMP_STATUS     2
#define DCMP_STATUS     3
#define DCMP_STATUS_SMP 4

#define ERRR_STR_STATUS     "Error"
#define HELP_STR_STATUS     "Help"
#define TEST_STR_STATUS     "Test"
#define COMP_STR_STATUS     "Compress"
#define DCMP_STR_STATUS     "Decompress (safe)"
#define DCMP_STR_STATUS_SMP "Decompress (simple)"

typedef struct {
    char* input_file;
    char* output_file;
    int status;
    bool verbose;
    int btstrm_ver;
} prs_args_t;

void show_help();

prs_args_t* prs_args_init();

void prs_args_free(prs_args_t* obj);

prs_args_t* parse_args(int argc, char *argv[]);

#endif /* PARSE_ARGS_H */