#include "interp.h"

int main (int argc, char** argv)
{
    if(argc < MIN_ARGS || argc > MAX_ARGS){
        fprintf(stderr, "Usage: %s <input file> (<output_file>)", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* f = fopen(argv[1], "r");
    program *start, *p, *previous;
    start = (program*)neill_calloc(1, sizeof(program));
    p = start;

    while(fscanf(f, "%s", p->word) != EOF){
        previous = p;
        p->next = (program*)neill_calloc(1, sizeof(program));
        p = p->next;
        p->previous = previous;

    }
    p = start;

    bool pass = prog(&p);

    char array[HEIGHT][WIDTH] = {{'\0'}};

    prog_to_array(start, array);

    print_array(array);

    prog_free(start);

    fclose(f);

    if(pass){
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

void* neill_calloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if(v==NULL){
    fprintf(stderr, "Cannot calloc() space\n");
    exit(EXIT_FAILURE);
   }
   return v;
}

bool prog_free(program* start)
{
    if(!start){
        return false;
    }

    program *temp;

    while(start->next){
        temp = start->next;
        free(start);
        start = temp;
    }

    free(start);

    return true;
}

bool prog(program** prog)
{
    if(!strsame((*prog)->word, "START")){
        ERROR("No START statement");
        return false; 
    }
    set_values(prog, false, START_ROW, START_COLUMN, 'W', PI/2);
    *prog = (*prog)->next;
    return inlist(prog);
}

bool inlist(program** prog)
{
    if(strsame((*prog)->word, "END")){
        set_prev_values(prog);
        return true;
    }
    if(!ins(prog)){
        return false;
    }
    *prog = (*prog)->next;
    return inlist(prog);
}

bool ins(program** prog)
{
    if(strsame((*prog)->word, "FORWARD")){
        return fwd(prog);
    }else if(strsame((*prog)->word, "RIGHT")){
        return rgt(prog);
    }else if(strsame((*prog)->word, "COLOUR")){
        return col(prog);
    }else if(strsame((*prog)->word, "LOOP")){
        return loop(prog);
    }else if(strsame((*prog)->word, "SET")){
        return set(prog);
    }else{
        ERROR("Expecting INS");
        return false;
    }

}

bool fwd(program** prog)
{  
    set_prev_values(prog);
    *prog = (*prog)->next;
    return varnum(prog);
}

bool rgt(program** prog)
{
    set_prev_values(prog);
    *prog = (*prog)->next;
    return varnum(prog);
}

bool col(program** prog)
{
    *prog = (*prog)->next;
    if((*prog)->word[0] == '$'){
        return var(prog);
    }else if((*prog)->word[0] == '"'){
        return word(prog);
    }else{
        ERROR("Expecting COL");
        return false;
    }
}

bool loop(program** prog)
{
    *prog = (*prog)->next;
    if(!ltr(prog)){
        return false;
    }
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "OVER")){
        ERROR("Expecting OVER");
        return false;
    }
    *prog = (*prog)->next;
    if(!lst(prog)){
        return false;
    }
    *prog = (*prog)->next;
    return inlist(prog);
}

bool var(program** prog)
{
    if((*prog)->word[0] != '$'){
        ERROR("Expecting VAR");
        return false;
    }
    return ltr(prog);

}

bool ltr(program** prog)
{
    if((*prog)->word[0] == '$'){
        if(!isupper((*prog)->word[1])){
            ERROR("Expecting LTR");
            return false;
        }
    }else{
        if(!isupper((*prog)->word[0])){
            ERROR("Expecting LTR");
            return false;
        }
    }
    return true;
}

bool lst(program** prog)
{
    if(!strsame((*prog)->word, "{")){
        ERROR("Expecting { for LST");
        return false;
    }
    *prog = (*prog)->next;
    return items(prog);
}

bool items(program** prog)
{
    if(strsame((*prog)->word, "}")){
        return true;
    }
    if(!item(prog)){
        return false;
    }
    *prog = (*prog)->next;
    return items(prog);
}

bool item(program** prog)
{
    if((*prog)->word[0] == '"'){
        return word(prog);
    }else{
        return varnum(prog);
    }
}

bool varnum(program** prog)
{
    if((*prog)->word[0] == '$'){
        return var(prog);
    }else if(isdigit((*prog)->word[0]) || (*prog)->word[0] == '-'){
        return num(prog);
    }else{
        ERROR("Expecting VARNUM");
        return false;
    }
}

bool num(program** prog)
{
    double n, rad = 0;

    if(sscanf((*prog)->word, "%lf", &n) != 1){
        ERROR("Expecting NUM");
        return false;
    }

    set_prev_values(prog);

    if((*prog)->previous->word[0] == 'R'){
        rad = deg_to_radians(n);
        (*prog)->facing = (*prog)->previous->facing + rad;
        (*prog)->facing = facing_adjust((*prog)->facing);
    }else if((*prog)->previous->word[0] == 'F'){
        (*prog)->facing = (*prog)->previous->facing;
        (*prog)->row = new_row(*prog, n);
        (*prog)->column = new_column(*prog, n);
    }

    return true;
}

bool set(program** prog)
{
    *prog = (*prog)->next;
    if(!ltr(prog)){
        return false;
    }
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "(")){
        ERROR("Expecting SET");
        return false;
    }
    *prog = (*prog)->next;
    return pfix(prog);
}

bool pfix(program** prog)
{
    if(strsame((*prog)->word,")")){
        return true;
    }else if(isop(prog)){
        if(!op(prog)){
            return false;
        }
    }else{
        if(!varnum(prog)){
            return false;
        }
    }
    *prog = (*prog)->next;
    return pfix(prog);
}

bool isop(program** prog)
{
    if(strsame((*prog)->word, "-")){
        return true;
    }else if(strsame((*prog)->word, "+")){
        return true;
    }else if(strsame((*prog)->word, "*")){
        return true;
    }else if(strsame((*prog)->word, "/")){
        return true;
    }
    return false;
}

bool op(program** prog)
{
    if(!isop(prog)){
        ERROR("Expecting OP");
        return false;
    }
    return true;
}

bool word(program** prog)
{
    int len = strlen((*prog)->word);
    if((*prog)->word[0] != '"' || (*prog)->word[(len - 1)] != '"'){
        ERROR("Expecting WORD");
        return false;
    }
    return true;
}

void set_values(program** prog, bool write, int row, int column, char colour, double facing)
{
    (*prog)->colour = colour;
    (*prog)->column = column;
    (*prog)->row = row;
    (*prog)->write = write;
    (*prog)->facing = facing;
}

void set_prev_values(program** prog)
{
    (*prog)->colour = (*prog)->previous->colour;
    (*prog)->column = (*prog)->previous->column;
    (*prog)->row = (*prog)->previous->row;
    (*prog)->write = false;
    (*prog)->facing = (*prog)->previous->facing;
}

double deg_to_radians(double deg)
{
    double rad = deg*PI/SEMI_CIRC;

    return rad;
}

double facing_adjust(double facing)
{
    if(facing > (2*PI)){
        facing = facing - (2*PI);
    }
    if(facing < 0 ){
        facing = facing + (2*PI);
    }

    return facing;
}

int new_row(program* prog, double n)
{
    int new_row;
    if(prog->facing <= PI/2){
        new_row = prog->previous->row - (int)opposite(n, prog->facing);
    }else if(prog->facing <= PI){
        new_row = prog->previous->row - (int)adjacent(n, (prog->facing - PI/2));
    }else if(prog->facing <= 1.5 * PI){
        new_row = prog->previous->row + (int)opposite(n, (prog->facing - PI));
    }else{
        new_row = prog->previous->row + (int)adjacent(n, (prog->facing - 1.5*PI));
    }

    return new_row;
}

int new_column(program* prog, double n)
{
    int new_col;
    if(prog->facing <= PI/2){
        new_col = prog->previous->column - (int)adjacent(n, prog->facing);
    }else if(prog->facing <= PI){
        new_col = prog->previous->column + (int)opposite(n, (prog->facing - PI/2));
    }else if(prog->facing <= 1.5 * PI){
        new_col = prog->previous->column + (int)adjacent(n, (prog->facing - PI));
    }else{
        new_col = prog->previous->column - (int)opposite(n, (prog->facing - 1.5*PI));
    }

    return new_col;
}

double opposite(int hypotenuse, double theta)
{
    double opposite;

    opposite = sin(theta) * hypotenuse;

    return round(opposite);
}

double adjacent(int hypotenuse, double theta)
{
    double adjacent;

    adjacent = cos(theta) * hypotenuse;

    return round(adjacent);
}

void prog_to_array(program* prog, char array[HEIGHT][WIDTH])
{
    
    while(prog){        
        array[prog->row][prog->column] = prog->colour;
        printf("row = %i, column = %i\n", prog->row, prog->column);
        prog = prog->next;
    }
}

void print_array(char array[HEIGHT][WIDTH])
{
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){

            if(array[i][j] == '\0'){
                printf(" ");
            }else{
                printf("%c", array[i][j]);
            }
        }
        printf("\n");
    }
}








