#include "interp.h"

int main (int argc, char** argv)
{
    if(argc < MIN_ARGS || argc > MAX_ARGS){
        fprintf(stderr, "Usage: %s <input file> (<output_file>)\n", argv[0]);
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
    
    turtle* t, *t_start = init_turtle();

    t = t_start;
    p = start;

    bool pass = prog(&p, &t);

    char array[HEIGHT][WIDTH] = {{'\0'}};

    turtle_to_array(t_start, array);

    print_array(array);

    prog_free(start);
    turtle_free(t_start);

    fclose(f);

    if(pass){
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
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

bool turtle_free(turtle* start)
{
    if(!start){
        return false;
    }

    turtle *temp;

    while(start->next){
        temp = start->next;
        free(start);
        start = temp;
    }

    free(start);

    return true;
}

bool prog(program** prog, turtle** t)
{
    if(!strsame((*prog)->word, "START")){
        ERROR("No START statement");
        return false; 
    }
    *prog = (*prog)->next;
    return inlist(prog, t);
}

bool inlist(program** prog, turtle** t)
{
    if(strsame((*prog)->word, "END")){
        return true;
    }
    if(!ins(prog, t)){
        return false;
    }
    *prog = (*prog)->next;
    return inlist(prog, t);
}

bool ins(program** prog, turtle** t)
{
    if(strsame((*prog)->word, "FORWARD")){
        return fwd(prog, t);
    }else if(strsame((*prog)->word, "RIGHT")){
        return rgt(prog, t);
    }else if(strsame((*prog)->word, "COLOUR")){
        return col(prog, t);
    }else if(strsame((*prog)->word, "LOOP")){
        return loop(prog, t);
    }else if(strsame((*prog)->word, "SET")){
        return set(prog, t);
    }else{
        ERROR("Expecting INS");
        return false;
    }

}

bool fwd(program** prog, turtle** t)
{  
    *prog = (*prog)->next;
    return varnum(prog, t);
}

bool rgt(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    return varnum(prog, t);
}

bool col(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    if((*prog)->word[0] == '$'){
        return var(prog, t);
    }else if((*prog)->word[0] == '"'){
        return word(prog, t);
    }else{
        ERROR("Expecting COL");
        return false;
    }
}

bool loop(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    if(!ltr(prog, t)){
        return false;
    }
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "OVER")){
        ERROR("Expecting OVER");
        return false;
    }
    *prog = (*prog)->next;
    if(!lst(prog, t)){
        return false;
    }
    *prog = (*prog)->next;
    return inlist(prog, t);
}

bool var(program** prog, turtle** t)
{
    if((*prog)->word[0] != '$'){
        ERROR("Expecting VAR");
        return false;
    }
    return ltr(prog, t);

}

bool ltr(program** prog, turtle** t)
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

bool lst(program** prog, turtle** t)
{
    if(!strsame((*prog)->word, "{")){
        ERROR("Expecting { for LST");
        return false;
    }
    *prog = (*prog)->next;
    // counter for loop members,
    static int i = 0;
    return items(prog, t, i);
}

bool items(program** prog, turtle** t, int i)
{
    if(strsame((*prog)->word, "}")){
        return true;
    }
    //static int i = 0;
    
    printf("i = %i\nword = %s\n", i, (*prog)->word);
    if(!item(prog, t)){
        return false;
    }
    i++;
    *prog = (*prog)->next;
    return items(prog, t, i);
}

bool item(program** prog, turtle** t)
{
    if((*prog)->word[0] == '"'){
        return word(prog, t);
    }else{
        return varnum(prog, t);
    }
}

bool varnum(program** prog, turtle** t)
{
    if((*prog)->word[0] == '$'){
        return var(prog, t);
    }else if(isdigit((*prog)->word[0]) || (*prog)->word[0] == '-'){
        return num(prog, t);
    }else{
        ERROR("Expecting VARNUM");
        return false;
    }
}

bool num(program** prog, turtle** t)
{
    double n, rad = 0;

    if(sscanf((*prog)->word, "%lf", &n) != 1){
        ERROR("Expecting NUM");
        return false;
    }

    if((*prog)->previous->word[0] == 'R'){
        rad = deg_to_radians(n);
        advance_turtle(t);
        set_prev_values(t);
        (*t)->facing = (*t)->previous->facing + rad;
        (*t)->facing = facing_adjust((*t)->facing);
    }else if((*prog)->previous->word[0] == 'F'){
        advance_turtle(t);
        set_prev_values(t);
        (*t)->row= new_row(*t, n);
        (*t)->column = new_column(*t, n);
    }

    return true;
}

bool set(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    if(!ltr(prog, t)){
        return false;
    }
    char var = (*prog)->word[0];
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "(")){
        ERROR("Expecting SET");
        return false;
    }
    *prog = (*prog)->next;
    stack* pfix_stack = stack_init();
    bool pass = pfix(prog, t, pfix_stack);
    char val[MAX_PFIX];
    stack_pop(pfix_stack, val);
    stack_free(pfix_stack);

    set_var(var, val, t);

    printf("%c = %s\n", var, (*t)->variables[(var-ASCII_A)]);
    
    return pass;
}

bool pfix(program** prog, turtle** t, stack* pfix_stack)
{
    char result[MAX_PFIX], v1[MAX_PFIX] = {'\0'}, v2[MAX_PFIX] = {'\0'};
    if(strsame((*prog)->word,")")){
        return true;
    }else if(isop(prog)){
        if(!op(prog, t)){
            return false;
        }
        stack_pop(pfix_stack, v2);
        stack_pop(pfix_stack, v1);
        if(v1[0] == '$'){
            var_get(v1, t);
        }
        if(v2[0] == '$'){
            var_get(v2, t);
        }
        calc_pfix(result, v1, v2, (*prog)->word[0]);
        stack_push(pfix_stack, result);
    }else{
        if(!varnum(prog, t)){
            return false;
        }
        stack_push(pfix_stack, (*prog)->word);
    }
    *prog = (*prog)->next;
    return pfix(prog, t, pfix_stack);
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

bool op(program** prog, turtle** t)
{
    if(!isop(prog)){
        ERROR("Expecting OP");
        return false;
    }
    return true;
}

bool word(program** prog, turtle** t)
{
    int len = strlen((*prog)->word);
    if((*prog)->word[0] != '"' || (*prog)->word[(len - 1)] != '"'){
        ERROR("Expecting WORD");
        return false;
    }
    return true;
}

void set_values(turtle** t, int row, int column, char colour, double facing)
{   
    (*t)->colour = colour;
    (*t)->column = column;
    (*t)->row = row;
    (*t)->facing = facing;

}

void set_prev_values(turtle** t)
{
    (*t)->colour = (*t)->previous->colour;
    (*t)->column = (*t)->previous->column;
    (*t)->row = (*t)->previous->row;
    (*t)->facing = (*t)->previous->facing;
    for(int i = 0; i < LETTERS; i++){
        strcpy((*t)->variables[i], (*t)->previous->variables[i]);
    }

}

void advance_turtle(turtle** t)
{
    turtle* prev = *t;
    (*t)->next = (turtle*)neill_calloc(1, sizeof(turtle));
    (*t) = (*t)->next;
    (*t)->previous = prev;
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

int new_row(turtle* t, double n)
{
    int new_row;
    if(t->facing <= PI/2){
        new_row = t->previous->row - (int)opposite(n, t->facing);
    }else if(t->facing <= PI){
        new_row = t->previous->row - (int)adjacent(n, (t->facing - PI/2));
    }else if(t->facing <= 1.5 * PI){
        new_row = t->previous->row + (int)opposite(n, (t->facing - PI));
    }else{
        new_row = t->previous->row + (int)adjacent(n, (t->facing - 1.5*PI));
    }

    return new_row;
}

int new_column(turtle* t, double n)
{
    int new_col;
    if(t->facing <= PI/2){
        new_col = t->previous->column - (int)adjacent(n, t->facing);
    }else if(t->facing <= PI){
        new_col = t->previous->column + (int)opposite(n, (t->facing - PI/2));
    }else if(t->facing <= 1.5 * PI){
        new_col = t->previous->column + (int)adjacent(n, (t->facing - PI));
    }else{
        new_col = t->previous->column - (int)opposite(n, (t->facing - 1.5*PI));
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

void turtle_to_array(turtle* t, char array[HEIGHT][WIDTH])
{
    
    while(t->next){        
        array[t->row][t->column] = t->colour;
        printf("row = %i, column = %i\n", t->row, t->column);
        t = t->next;
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

turtle* init_turtle(void){
    turtle* t = (turtle*)neill_calloc(1, sizeof(turtle));
    t->colour = 'W';
    t->column = START_COLUMN;
    t->facing = PI/2;
    t->row = START_ROW;

    return t;
}

bool set_var(char var, char val[MAX_PFIX], turtle** t)
{
    if(!t || !(*t)){
        return false;
    }
    int row = var - ASCII_A;
    strcpy((*t)->variables[row], val);
    return true;
}

void calc_pfix(char result[MAX_PFIX], char v1[MAX_PFIX], char v2[MAX_PFIX], char op)
{
    double x1, x2, res_int;

    x1 = atof(v1);
    x2 = atof(v2);

    switch(op){
        case '+':
            res_int = x1 + x2;
            break;
        case '-':
            res_int = x1 - x2;
            break;
        case '/':
            res_int = x1 / x2;
            break;
        case '*':
            res_int = x1 * x2;
            break;
        default:
               fprintf(stderr, "Can't understand that ? %c\n", op);
               exit(EXIT_FAILURE);
    }
    sprintf(result, "%lf", res_int);

}

void var_get(char var[LONGEST_WORD], turtle** t)
{
    int row = var[1] - ASCII_A;

    strcpy(var, (*t)->variables[row]);
}

