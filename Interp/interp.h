#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LONGEST_WORD 20
#define MIN_ARGS 2
#define MAX_ARGS 3
#define WIDTH 51
#define HEIGHT 33
#define START_ROW 17
#define START_COLUMN 26
#define PI 3.14159265
#define SEMI_CIRC 180


#define strsame(A, B) (strcmp(A, B) == 0)
#define ERROR(PHRASE) { fprintf(stderr, \
        "Fatal Error %s occurred in %s, line %d\n", PHRASE, \
        __FILE__, __LINE__); \
        exit(EXIT_FAILURE); }

struct program {
    char word[LONGEST_WORD];
    struct program* next;
    struct program* previous;
    bool write;
    int row;
    int column;
    char colour;
    double facing;
};

typedef struct program program;

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

bool ltr(program** prog);

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

void set_values(program** prog, bool write, int row, int column, char colour, double facing);

void set_prev_values(program** prog);

double deg_to_radians(double deg);

double facing_adjust(double facing);

int new_row(program* prog, double n);

int new_column(program* prog, double n);

double opposite(int hypotenuse, double theta);

double adjacent(int hypotenuse, double theta);

void prog_to_array(program* prog, char array[HEIGHT][WIDTH]);

void print_array(char array[HEIGHT][WIDTH]);
