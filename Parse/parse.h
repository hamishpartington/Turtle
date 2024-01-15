#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>

#define LONGEST_WORD 20
#define NUM_ARGS 2
#define PRODUCTION 0

#define strsame(A, B) (strcmp(A, B) == 0)
#define ERROR(PHRASE) { fprintf(stdout, \
        "Parsing Error: %s occurred in %s function\n", PHRASE, __func__); }

struct program {
    char word[LONGEST_WORD];
    struct program* next;
};

typedef struct program program;

program* build_program(FILE* f);

void test(void);

//Stolen from Neill (https://github.com/csnwc/ADTs/blob/main/General/general.c)
void* neill_calloc(int n, size_t size);

bool prog_free(program* start);

bool prog(program** prog);

bool inlist(program** prog);

bool ins(program** prog);

bool fwd(program** prog);

bool rgt(program** prog);

bool col(program** prog);

bool loop(program** prog);

bool ltr(program** prog, bool isvar);

bool var(program** prog);

bool lst(program** prog);

bool items(program** prog);

bool item(program** prog);

bool varnum(program** prog);

bool num(program** prog);

bool set(program** prog);

bool pfix(program** prog);

bool word(program** prog);

bool isop(program** prog);

bool op(program** prog);

void buff_reset(char buffer[BUFSIZ]);
