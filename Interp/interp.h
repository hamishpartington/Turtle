#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "./Stack/stack.h"

#define LONGEST_WORD 20
#define MAX_LOOPS 50
#define MIN_ARGS 2
#define MAX_ARGS 3
#define WIDTH 51
#define HEIGHT 33
#define START_ROW (HEIGHT/2)
#define START_COLUMN (WIDTH/2)
#define PI 3.14159265
#define SEMI_CIRC 180
#define LETTERS 26
#define ASCII_A 65
#define OUT_FILE_ARG 2
#define PS_X_START 30
#define PS_Y_START 40
#define RGB_BLUE "0 0 1"
#define RGB_BLACK "0 0 0"
#define RGB_RED "1 0 0"
#define RGB_CYAN "0 1 1"
#define RGB_GREEN "0 1 0"
#define RGB_MAGENTA "1 0 1"
#define RGB_YELLOW "1 1 0"
#define RGB_WHITE "0.8 0.8 0.8"
#define SCALE 10
#define LINEWIDTH 0.2
#define MAX_EXT 4
#define MAX_DIR 100
#define PDF_LEN 4


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
    double ps_x;
    double ps_y;
    char colour;
    double facing;
    char variables[LETTERS][LONGEST_WORD];
    struct turtle* next;
    struct turtle* previous;
};

typedef struct program program;
typedef struct turtle turtle;

//Stolen from Neill (https://github.com/csnwc/ADTs/blob/main/General/general.c)


bool prog_free(program* start);

bool prog(program** prog, turtle** t);

bool inlist(program** prog, turtle** t);

bool ins(program** prog, turtle** t);

bool fwd(program** prog, turtle** t);

bool rgt(program** prog, turtle** t);

bool col(program** prog, turtle** t);

bool loop(program** prog, turtle** t);

bool ltr(program** prog);

bool var(program** prog, turtle** t);

bool lst(program** prog, turtle** t, char loop_var_list[MAX_LOOPS][LONGEST_WORD]);

bool items(program** prog, turtle** t, int i, char loop_var_list[MAX_LOOPS][LONGEST_WORD]);

bool item(program** prog, turtle** t);

bool varnum(program** prog, turtle** t);

bool num(program** prog, turtle** t);

bool set(program** prog, turtle** t);

bool pfix(program** prog, turtle** t, stack* pfix_stack);

bool word(program** prog);

bool isop(program** prog);

bool op(program** prog);

void set_prev_values(turtle** t);

double deg_to_radians(double deg);

double facing_adjust(double facing);

int new_row(turtle* t, double n);

int new_column(turtle* t, double n);

double opposite(double hypotenuse, double theta);

double adjacent(double hypotenuse, double theta);

void turtle_to_array(turtle* t, char array[HEIGHT][WIDTH], bool console);

void print_to_console(char array[HEIGHT][WIDTH]);

void print_to_file(char array[HEIGHT][WIDTH], FILE* out_file);

void advance_turtle(turtle** t);

turtle* init_turtle(void);

bool turtle_free(turtle* start);

bool set_var(char var, char val[MAX_PFIX], turtle** t);

void calc_pfix(char result[MAX_PFIX], char v1[MAX_PFIX], char v2[MAX_PFIX], char op);

bool var_get(char var[LONGEST_WORD], turtle** t);

bool set_col(turtle** t, char word[LONGEST_WORD], bool is_var);

void line_draw(char array[HEIGHT][WIDTH], double sx, double sy, double ex, double ey, char colour);

void change_col(char c);

double new_ps_y(turtle* t, double n);

double new_ps_x(turtle* t, double n);

void turtle_to_ps(turtle* t, FILE* ps_output);

void set_ps_colour(char colour, FILE* ps_output);

bool turtle_moves(turtle* t);

bool get_file_ext(char* fname, char file_ext[MAX_EXT]);

void ps_to_pdf(char fname[MAX_DIR]);
