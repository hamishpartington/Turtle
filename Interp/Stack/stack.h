//these functions are all from Neill's github

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FORMATSTR "%i"
#define ELEMSIZE 20
#define STACKTYPE "Linked"
#define MAX_PFIX 20

struct dataframe {
   char c[MAX_PFIX];
   struct dataframe* next;
};
typedef struct dataframe dataframe;

struct stack {
   dataframe* start;
   int size;
};

typedef struct stack stack;

void* neill_calloc(int n, size_t size);

stack* stack_init(void);

void stack_push(stack* s, char data[MAX_PFIX]);

bool stack_pop(stack* s, char* d);

bool stack_free(stack* s);