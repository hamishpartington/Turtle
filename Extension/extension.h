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
#define BLACK 0,0,0
#define WHITE 255,255,255
#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define GREY 200,200,200
#define CYAN 0,255,255
#define MAGENTA 255,0,255
#define YELLOW 255,255,0
#define OPAQUE 255
#define START_X WIDTH/2
#define START_Y HEIGHT/2
#define SIDELENGTH 10
#define DARK_GREEN 1,50,32
#define MAX_POINTS 1000
#define MOVE_DIST 20
#define PI 3.14159265
#define SEMI_CIRC 180
#define RAD PI/SEMI_CIRC
#define MAX_INS 20
#define ARGS 2;

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

