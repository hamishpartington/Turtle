#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LONGEST_WORD 20
#define MAX_LOOPS 50
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
};

struct turtle {
    int row;
    int column;
    char colour;
    double facing;
    struct turtle* next;
    struct turtle* previous;
};

typedef struct program program;
typedef struct turtle turtle;

//Stolen from Neill (https://github.com/csnwc/ADTs/blob/main/General/general.c)
void* neill_calloc(int n, size_t size);

bool prog_free(program* start);

bool prog(program** prog, turtle** t);

bool inlist(program** prog, turtle** t);

bool ins(program** prog, turtle** t);

bool fwd(program** prog, turtle** t);

bool rgt(program** prog, turtle** t);

bool col(program** prog, turtle** t);

bool loop(program** prog, turtle** t);

bool ltr(program** prog, turtle** t);

bool var(program** prog, turtle** t);

bool lst(program** prog, turtle** t);

bool items(program** prog, turtle** t, int i);

bool item(program** prog, turtle** t);

bool varnum(program** prog, turtle** t);

bool num(program** prog, turtle** t);

bool set(program** prog, turtle** t);

bool pfix(program** prog, turtle** t);

bool word(program** prog, turtle** t);

bool isop(program** prog);

bool op(program** prog, turtle** t);

void set_values(turtle** t, int row, int column, char colour, double facing);

void set_prev_values(turtle** t);

double deg_to_radians(double deg);

double facing_adjust(double facing);

int new_row(turtle* t, double n);

int new_column(turtle* t, double n);

double opposite(int hypotenuse, double theta);

double adjacent(int hypotenuse, double theta);

void turtle_to_array(turtle* t, char array[HEIGHT][WIDTH]);

void print_array(char array[HEIGHT][WIDTH]);

void advance_turtle(turtle** t);

turtle* init_turtle(void);

bool turtle_free(turtle* start);
