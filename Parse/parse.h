#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LONGEST_WORD 20

#define strsame(A, B) (strcmp(A, B) == 0)
#define ERROR(PHRASE) { fprintf(stderr, \
        "Fatal Error %s occurred in %s, line %d\n", PHRASE, \
        __FILE__, __LINE__); \
        exit(EXIT_FAILURE); }

struct program {
    char word[LONGEST_WORD];
    struct program* next;
};

typedef struct program program;

//Stolen from Neill (https://github.com/csnwc/ADTs/blob/main/General/general.c)
void* neill_calloc(int n, size_t size);

bool prog_free(program* start);

void prog(program** prog);

void inlist(program** prog);

void ins(program** prog);

void fwd(program** prog);

void rgt(program** prog);

void col(program** prog);

void loop(program** prog);

void ltr(program** prog);

void var(program** prog);

void lst(program** prog);

void items(program** prog);

void item(program** prog);

void varnum(program** prog);

void num(program** prog);

void set(program** prog);

void pfix(program** prog);

void word(program** prog);

bool isop(program** prog);

void op(program** prog);
