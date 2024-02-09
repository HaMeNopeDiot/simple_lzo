#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERRR_STATUS -1
#define HELP_STATUS 0
#define TEST_STATUS 1
#define COMP_STATUS 2
#define DCMP_STATUS 3

typedef struct {
    char* input_file;
    char* output_file;
    int status;
} prs_args_t;

void show_help();

prs_args_t* prs_args_init();

void prs_args_free(prs_args_t* obj);

prs_args_t* parse_args(int argc, char *argv[]);

#endif /* PARSE_ARGS_H */