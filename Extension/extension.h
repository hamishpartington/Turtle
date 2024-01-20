#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <SDL.h>
#include <math.h>

#define HEIGHT 660
#define WIDTH 1020
#define RGB_BLACK 0,0,0
#define RGB_WHITE 255,255,255
#define RGB_RED 255,0,0
#define RGB_GREEN 0,255,0
#define RGB_BLUE 0,0,255
#define RGB_GREY 200,200,200
#define RGB_CYAN 0,255,255
#define RGB_MAGENTA 255,0,255
#define RGB_YELLOW 255,255,0
#define OPAQUE 255
#define START_X WIDTH/2
#define START_Y HEIGHT/2
#define SIDELENGTH 10
#define RGB_DARK_GREEN 1,50,32
#define MAX_POINTS 1000
#define MOVE_DIST 20
#define PI 3.14159265
#define SEMI_CIRC 180
#define RAD PI/SEMI_CIRC
#define MAX_INS 20
#define ARGS 2
#define MAX_COMMAND 100
#define TWO_PI 2*PI
#define HALF_PI PI/2
#define THREE_HALF_PI PI*1.5
#define BLUE 'B'
#define BLACK 'K'
#define RED 'R'
#define CYAN 'C'
#define GREEN 'G'
#define MAGENTA 'M'
#define YELLOW 'Y'
#define WHITE 'W'
#define COLOUR 'C'

struct lines {
    double x;
    double y;
    char colour;
    double facing;
    char instruct[MAX_INS];
    double val;
    struct lines* previous;
    struct lines* next;
};
typedef struct lines lines;

#define strsame(A, B) (strcmp(A, B) == 0)

int SDL_display_turtle(lines *l);

lines* init_lines(void);

SDL_Rect move_turtle(lines **l, int key_inp, SDL_Rect turtle);

double opposite(double hypotenuse, double theta);

double adjacent(double hypotenuse, double theta);

double facing_adjust(double facing);

bool lines_free(lines* start);

double new_x(lines* l, double n);

double new_y(lines* l, double n);

void* neill_calloc(int n, size_t size);

void advance_line(lines** t);

void set_prev_values(lines** t);

void render_line(lines* start, SDL_Renderer *renderer);

void set_colour(SDL_Renderer *renderer, char colour);

void output_ttl(lines* start);

