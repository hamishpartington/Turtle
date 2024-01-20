#include "interp.h"
#include "../neillsimplescreen.h"

int main (int argc, char** argv)
{
    if(argc < MIN_ARGS || argc > MAX_ARGS){
        fprintf(stderr, "Usage: %s <input file> (<output_file>)\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(!PRODUCTION){
        test();
    }

    FILE* f = fopen(argv[1], "r");
    program *start, *p;

    start = build_program(f);
    fclose(f);
    p = start;
    
    turtle* t, *t_start = init_turtle();
    t = t_start;

    bool pass = prog(&p, &t);

    if(!output_func(argc, t_start, argv)){
        pass = false;
    }

    prog_free(start);
    turtle_free(t_start);

    if(pass){
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool output_func(int argc, turtle* t_start, char** argv)
{
    char array[HEIGHT][WIDTH] = {{'\0'}};

    if(argc == MIN_ARGS){
        turtle_to_array(t_start, array, true);
        return true;
    }else{
        turtle_to_array(t_start, array, false);
        char out_dir[MAX_DIR] = {'\0'};
        strcpy(out_dir, "Results/");

        char file_ext[MAX_EXT] = {'\0'};
        get_file_ext(argv[OUT_FILE_ARG], file_ext);

        FILE* out_file = fopen(strcat(out_dir, argv[OUT_FILE_ARG]), "w+");

        if(strsame(file_ext, "txt")){
            print_to_file(array, out_file);
            fclose(out_file);
            return true;
        }else if(strsame(file_ext, "ps")){
            turtle_to_ps(t_start, out_file);
            fclose(out_file);
            char system_command[MAX_DIR] = {'\0'};
            strcat(system_command, "ps2pdf ");
            strcat(system_command, out_dir);
            strcat(system_command, " ");
            ps_to_pdf(out_dir);
            strcat(system_command, out_dir);
            system(system_command);
            return true;
        }else{
            fprintf(stderr, "%s is not a valid output file type\n", file_ext);
            fclose(out_file);
            return false;
        }       
    }
}

program* build_program(FILE* f)
{
    if(!f){
        ERROR("Input NULL FILE pointer");
        exit(EXIT_FAILURE);
    }
    program *start, *p, *previous;
    start = (program*)neill_calloc(1, sizeof(program));
    p = start;

    while(fscanf(f, "%s", p->word) != EOF){
        previous = p;
        p->next = (program*)neill_calloc(1, sizeof(program));
        p = p->next;
        p->previous = previous;
    }
    return start;
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
    if(!(*prog) || !(*t)){
        ERROR("Null pointer input");
        return false;
    }
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
    //catch programs with no end
    if((*prog)->next == NULL && !strsame((*prog)->word, "END")){
        ERROR("No end to program");
        return false;
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
        return (set_col(t, (*prog)->word, true) && var(prog, t));
    }else if((*prog)->word[0] == '"'){
        return (set_col(t, (*prog)->word, false) && word(prog));
    }else{
        ERROR("Expecting COL");
        return false;
    }
}

bool loop(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    if(!ltr(prog, false)){
        return false;
    }
    char loop_var = (*prog)->word[0];
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "OVER")){
        ERROR("Expecting OVER");
        return false;
    }
    *prog = (*prog)->next;
    char loop_var_list[MAX_LOOPS][LONGEST_WORD] = {{'\0'}};
    
    if(!lst(prog, t, loop_var_list)){
        return false;
    }
    *prog = (*prog)->next;
    program* loop_start = *prog;
    bool pass = true;
    //if no items in loop we need to skip to end
    if(loop_var_list[0][0] == '\0'){
        pass = skip_to_loop_end(prog);
    }else{
        pass = check_for_loop_end(*prog);
    }
    if(!pass){
        return false;
    }
    for(int i = 0; loop_var_list[i][0] != '\0'; i++){
        *prog = loop_start;
        pass = (set_var(loop_var, loop_var_list[i], t) && inlist(prog, t));
        //to catch when setting 1 element of loop fails but not all
        if(!pass){
            return false;
        }
    }
    return pass;
}

bool var(program** prog, turtle** t)
{
    if((*prog)->word[0] != '$'){
        ERROR("Expecting VAR");
        return false;
    }

    char var[LONGEST_WORD];
    strcpy(var, (*prog)->word);
    bool get_success = var_get(var, t);

    double rad, n = atof(var);

    if((*prog)->previous->word[0] == RIGHT){
        rad = deg_to_radians(n);
        advance_turtle(t);
        set_prev_values(t);
        (*t)->facing = (*t)->previous->facing + rad;
        (*t)->facing = facing_adjust((*t)->facing);
    }else if((*prog)->previous->word[0] == FORWARD){
        advance_turtle(t);
        set_prev_values(t);
        (*t)->row = new_row(*t, n);
        (*t)->column = new_column(*t, n);
        (*t)->ps_x = new_ps_x(*t, n);
        (*t)->ps_y = new_ps_y(*t, n);
    }

    return (get_success && ltr(prog, true));

}

bool ltr(program** prog, bool isvar)
{
    int len = strlen((*prog)->word);
    if(len > VAR_LEN){
            ERROR("Expecting LTR (string too long)");
            return false;
    }
    if(isvar && len == VAR_LEN){
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

bool lst(program** prog, turtle** t, char loop_var_list[MAX_LOOPS][LONGEST_WORD])
{
    if(!strsame((*prog)->word, "{")){
        ERROR("Expecting { for LST");
        return false;
    }
    static int i = 0;
    *prog = (*prog)->next;
    return items(prog, t, i, loop_var_list);
}

bool items(program** prog, turtle** t, int i, char loop_var_list[MAX_LOOPS][LONGEST_WORD])
{
    if(strsame((*prog)->word, "}")){
        return true;
    }
    
    if(!item(prog, t)){
        return false;
    }
    if((*prog)->word[0] == VAR){
        strcpy(loop_var_list[i], (*t)->variables[((*prog)->word[1] - ASCII_A)]);
    }else{
        strcpy(loop_var_list[i], (*prog)->word);
    }    
    i++;
    *prog = (*prog)->next;
    return items(prog, t, i, loop_var_list);
}

bool item(program** prog, turtle** t)
{
    if((*prog)->word[0] == WORD){
        return word(prog);
    }else{
        return varnum(prog, t);
    }
}

bool varnum(program** prog, turtle** t)
{
    if((*prog)->word[0] == VAR){
        return var(prog, t);
    }else if(isdigit((*prog)->word[0]) || (*prog)->word[0] == NEGATIVE){
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

    if((*prog)->previous->word[0] == RIGHT){
        rad = deg_to_radians(n);
        advance_turtle(t);
        set_prev_values(t);
        (*t)->facing = (*t)->previous->facing + rad;
        (*t)->facing = facing_adjust((*t)->facing);
    }else if((*prog)->previous->word[0] == FORWARD){
        advance_turtle(t);
        set_prev_values(t);
        (*t)->row = new_row(*t, n);
        (*t)->column = new_column(*t, n);
        (*t)->ps_x = new_ps_x(*t, n);
        (*t)->ps_y = new_ps_y(*t, n);
    }

    return true;
}

bool set(program** prog, turtle** t)
{
    *prog = (*prog)->next;
    if(!ltr(prog, false)){
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
    pass = (pass && stack_pop(pfix_stack, val));
    stack_free(pfix_stack);
    
    return (set_var(var, val, t) && pass);
}

bool pfix(program** prog, turtle** t, stack* pfix_stack)
{
    char result[MAX_PFIX], v1[MAX_PFIX] = {'\0'}, v2[MAX_PFIX] = {'\0'};
    bool pass = true;
    if(strsame((*prog)->word,")")){
        return true;
    }else if(isop(prog)){
        if(!op(prog)){
            return false;
        }
        pass = stack_pop(pfix_stack, v2);
        pass = (pass && stack_pop(pfix_stack, v1));
        if(v1[0] == VAR){
            pass = (pass && var_get(v1, t));
        }
        if(v2[0] == VAR){
            pass = (pass && var_get(v2, t));
        }
        pass = (pass && calc_pfix(result, v1, v2, (*prog)->word[0]));
        stack_push(pfix_stack, result);
    }else{
        if(!varnum(prog, t)){
            return false;
        }
        stack_push(pfix_stack, (*prog)->word);
    }
    *prog = (*prog)->next;
    return (pass && pfix(prog, t, pfix_stack));
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
    if((*prog)->word[0] != WORD || (*prog)->word[(len - 1)] != WORD){
        ERROR("Expecting WORD");
        return false;
    }
    return true;
}

void set_prev_values(turtle** t)
{
    (*t)->colour = (*t)->previous->colour;
    (*t)->column = (*t)->previous->column;
    (*t)->row = (*t)->previous->row;
    (*t)->facing = (*t)->previous->facing;
    (*t)->ps_x = (*t)->previous->ps_x;
    (*t)->ps_y = (*t)->previous->ps_y;
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
    if(facing > (TWO_PI)){
        facing = facing - (TWO_PI);
    }
    if(facing < 0 ){
        facing = facing + (TWO_PI);
    }

    return facing;
}

int new_row(turtle* t, double n)
{
    int new_row;
    if(t->facing <= HALF_PI){
        new_row = t->previous->row - (int)round(opposite(n, t->facing));
    }else if(t->facing <= PI){
        new_row = t->previous->row - (int)round(adjacent(n, (t->facing - HALF_PI)));
    }else if(t->facing <= THREE_HALF_PI){
        new_row = t->previous->row + (int)round(opposite(n, (t->facing - PI)));
    }else{
        new_row = t->previous->row + (int)round(adjacent(n, (t->facing - THREE_HALF_PI)));
    }

    return new_row;
}

int new_column(turtle* t, double n)
{
    int new_col;
    if(t->facing <= HALF_PI){
        new_col = t->previous->column - (int)round(adjacent(n, t->facing));
    }else if(t->facing <= PI){
        new_col = t->previous->column + (int)round(opposite(n, (t->facing - HALF_PI)));
    }else if(t->facing <= THREE_HALF_PI){
        new_col = t->previous->column + (int)round(adjacent(n, (t->facing - PI)));
    }else{
        new_col = t->previous->column - (int)round(opposite(n, (t->facing - THREE_HALF_PI)));
    }

    return new_col;
}

double new_ps_y(turtle* t, double n)
{
    double new_ps_y;
    if(t->facing <= HALF_PI){
        new_ps_y = t->previous->ps_y + opposite(n, t->facing);
    }else if(t->facing <= PI){
        new_ps_y = t->previous->ps_y + adjacent(n, (t->facing - HALF_PI));
    }else if(t->facing <= THREE_HALF_PI){
        new_ps_y = t->previous->ps_y - opposite(n, (t->facing - PI));
    }else{
        new_ps_y = t->previous->ps_y - adjacent(n, (t->facing - THREE_HALF_PI));
    }

    return new_ps_y;
}

double new_ps_x(turtle* t, double n)
{
    double new_ps_x;
    if(t->facing <= HALF_PI){
        new_ps_x = t->previous->ps_x - adjacent(n, t->facing);
    }else if(t->facing <= PI){
        new_ps_x = t->previous->ps_x + opposite(n, (t->facing -HALF_PI));
    }else if(t->facing <= THREE_HALF_PI){
        new_ps_x = t->previous->ps_x + adjacent(n, (t->facing - PI));
    }else{
        new_ps_x = t->previous->ps_x - opposite(n, (t->facing - THREE_HALF_PI));
    }

    return new_ps_x;
}


double opposite(double hypotenuse, double theta)
{
    double opposite;

    opposite = sin(theta) * hypotenuse;

    return opposite;
}

double adjacent(double hypotenuse, double theta)
{
    double adjacent;

    adjacent = cos(theta) * hypotenuse;

    return adjacent;
}

bool in_bounds(int row, int col)
{
    if(row < HEIGHT && row >=0 && col < WIDTH && col >= 0){
        return true;
    }
    return false;
}

void turtle_to_array(turtle* t, char array[HEIGHT][WIDTH], bool console)
{
    double sx, sy, ex, ey;
    while(t->next){      
        if(in_bounds(t->row, t->column)){
            array[t->row][t->column] = t->colour;
        }
        sy = t->row;
        ey = t->next->row;
        sx = t->column;
        ex = t->next->column;
        line_draw(array, sx, sy, ex, ey, t->colour);
        if(console && ((int)(sx - ex) != 0 || (int)(sy - ey) != 0)){
            neillclrscrn();
            print_to_console(array);
            neillbusywait(1);
        }
        t = t->next;
    }
}

void print_to_file(char array[HEIGHT][WIDTH], FILE* out_file)
{
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){

            if(array[i][j] == '\0'){
                fprintf(out_file, " ");
            }else{
                fprintf(out_file, "%c", array[i][j]);
            }
        }
        fprintf(out_file, "\n");
    }
}

void print_to_console(char array[HEIGHT][WIDTH])
{
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){

            if(array[i][j] == '\0'){
                printf(" ");
            }else{
                change_col(array[i][j]);
                printf("%c", array[i][j]);
            }
        }
        printf("\n");
    }
    neillreset();
}

void change_col(char c)
{
    switch (c)
    {
    case BLACK:
        neillfgcol(black);
        break;
    case RED:
        neillfgcol(red);
        break;
    case GREEN:
        neillfgcol(green);
        break;
    case YELLOW:
        neillfgcol(yellow);
        break;
    case BLUE:
        neillfgcol(blue);
        break;
    case MAGENTA:
        neillfgcol(magenta);
        break;
    case CYAN:
        neillfgcol(cyan);
        break;
    case WHITE:
        neillfgcol(white);
        break;
    default:
        break;
    }
}

turtle* init_turtle(void){
    turtle* t = (turtle*)neill_calloc(1, sizeof(turtle));
    t->colour = WHITE;
    t->column = START_COLUMN;
    t->facing = HALF_PI;
    t->row = START_ROW;
    t->ps_x = PS_X_START;
    t->ps_y = PS_Y_START;

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

bool calc_pfix(char result[MAX_PFIX], char v1[MAX_PFIX], char v2[MAX_PFIX], char op)
{
    double x1, x2, res_int;

    x1 = atof(v1);
    x2 = atof(v2);

    switch(op){
        case ADD:
            res_int = x1 + x2;
            break;
        case NEGATIVE:
            res_int = x1 - x2;
            break;
        case DIVIDE:
            res_int = x1 / x2;
            break;
        case MULTIPLY:
            res_int = x1 * x2;
            break;
        default:
               fprintf(stdout, "Can't understand that operator: %c\n", op);
               return false;
               
    }
    sprintf(result, "%lf", res_int);
    return true;

}

bool var_get(char var[LONGEST_WORD], turtle** t)
{
    int row = var[1] - ASCII_A;

    if((*t)->variables[row][0] == '\0'){
        ERROR("Attempting to access unassigned variable");
        return false;
    }

    strcpy(var, (*t)->variables[row]);
    return true;
}

bool set_col(turtle** t, char word[LONGEST_WORD], bool is_var){
    
    char colour[LONGEST_WORD];

    if(is_var){
        int row = word[1] - ASCII_A;
        strcpy(colour, (*t)->variables[row]);
    }else{
        strcpy(colour, word);
    }

    if(strsame(colour, "\"RED\"")){
        (*t)->colour = RED;
    }else if(strsame(colour, "\"CYAN\"")){
        (*t)->colour = CYAN;
    }else if(strsame(colour, "\"GREEN\"")){
        (*t)->colour = GREEN;
    }else if(strsame(colour, "\"BLUE\"")){
        (*t)->colour = BLUE;
    }else if(strsame(colour, "\"YELLOW\"")){
        (*t)->colour = YELLOW;
    }else if(strsame(colour, "\"MAGENTA\"")){
        (*t)->colour = MAGENTA;
    }else if(strsame(colour, "\"BLACK\"")){
        (*t)->colour = BLACK;
    }else if(strsame(colour, "\"WHITE\"")){
        (*t)->colour = WHITE;
    }else{
        fprintf(stdout, "Interpretor Error: %s is not a valid colour in %s function\n", 
        colour, __func__);
        return false;
    }
    return true;
}

void line_draw(char array[HEIGHT][WIDTH], double sx, double sy, double ex, double ey, char colour)
{
    double dx, dy, dist, distdy, distdx, nx, ny;

    dx = ex - sx;
    dy = ey - sy;
    dist = sqrt((pow(dx, SQUARE) + pow(dy, SQUARE)));
    distdx = dx/dist;
    distdy = dy/dist;

    if(dist > 0){
        for(int i = 0; i <= (int)round(dist); i++){
            nx = round(sx + (distdx*i));
            ny = round(sy + (distdy*i));
            if(in_bounds((int)ny, (int)nx)){
                array[(int)ny][(int)nx] = colour;
            }
        }
    }
}

void turtle_to_ps(turtle* t, FILE* ps_output)
{
    //for start
    fprintf(ps_output, "%.1lf setlinewidth\n", LINEWIDTH);
    fprintf(ps_output, "%i %i scale\n", SCALE, SCALE);

    while(t->next){
        if(turtle_moves(t)){
            fprintf(ps_output, "newpath\n");
            fprintf(ps_output, "%lf %lf moveto\n", t->ps_x, t->ps_y);
            fprintf(ps_output, "%lf %lf lineto\n", t->next->ps_x, t->next->ps_y);
            set_ps_colour(t->colour, ps_output);
            fprintf(ps_output, "stroke\n");
        }
        t = t->next;
    }
    fprintf(ps_output, "showpage\n");
}

bool set_ps_colour(char colour, FILE* ps_output)
{
    switch (colour)
    {
    case WHITE:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_WHITE);
        return true;

    case BLUE:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_BLUE);
        return true;

    case BLACK:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_BLACK);
        return true;

    case CYAN:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_CYAN);
        return true;

    case YELLOW:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_YELLOW);
        return true;

    case GREEN:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_GREEN);
        return true;

    case RED:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_RED);
        return true;

    case MAGENTA:
        fprintf(ps_output, "%s setrgbcolor\n", RGB_MAGENTA);
        return true;
    default:
        fprintf(stderr, "%c is not a valid colour\n", colour);
    return false;
    }
}

bool turtle_moves(turtle* t)
{
    bool x_change = !(t->column == t->next->column);
    bool y_change = !(t->row == t->next->row);

    if(x_change || y_change){
        return true;
    }
    return false;
}

bool get_file_ext(char* fname, char file_ext[MAX_EXT])
{
    bool ext = false;
    int j = 0;
    for(int i = 0; fname[i] != '\0'; i++){
        if(j >= MAX_EXT){
            fprintf(stderr, "Invalid file extension\n");
            return false;
        }
        if(ext){
            file_ext[j] = fname[i];
            j++;
        }
        if(fname[i] == EXT){
            ext = true;
        }
    }
    return true;
}

void ps_to_pdf(char fname[MAX_DIR])
{
    bool ext = false;
    int j = 0, i;
    char pdf[PDF_LEN] = "pdf";
    for(i = 0; j < PDF_LEN; i++){
        if(ext){
            fname[i] = pdf[j];
            j++;
        }
        if(fname[i] == EXT){
            ext = true;
        }
    }
}

void buff_reset(char buffer[BUFSIZ])
{
    freopen("NUL", "a", stdout);
    memset(buffer, 0, BUFSIZ);
    setvbuf(stdout, buffer, _IOFBF, BUFSIZ);
}

bool skip_to_loop_end(program** prog){
    int offset = 0;
    while(!strsame((*prog)->word, "END") || offset != 0){
        if(strsame((*prog)->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
        if(strsame((*prog)->word, "LOOP")){
            offset++;
        }
        *prog = (*prog)->next;
        if(strsame((*prog)->word, "END") && strsame((*prog)->next->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
        if(strsame((*prog)->word, "END") && offset > 0){
            offset--;
            *prog = (*prog)->next;
        }
        if(strsame((*prog)->word, "END") && strsame((*prog)->next->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
    }
    return true;
}

bool check_for_loop_end(program* prog){
    int offset = 0;
    while(!strsame(prog->word, "END") || offset != 0){
        if(strsame(prog->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
        if(strsame(prog->word, "LOOP")){
            offset++;
        }
        prog = prog->next;
        if(strsame(prog->word, "END") && strsame(prog->next->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
        if(strsame(prog->word, "END") && offset > 0){
            offset--;
            prog = prog->next;
        }
        if(strsame(prog->word, "END") && strsame(prog->next->word, "")){
            ERROR("LOOP has no END");
            return false;
        }
    }
    return true;
}


void test(void)
{
    program* start, *p = NULL;

    assert(!prog_free(p));
    p = (program*)neill_calloc(1, sizeof(program));
    start = p;
    assert(prog_free(start));

    char pfix_result[MAX_PFIX];
    int x;

    //code to redirect stdout to buffer so error messages can be tested
    char buffer[BUFSIZ];
    fflush(stdout);
    int stdout_save = dup(STDOUT_FILENO);
    freopen("NUL", "a", stdout);
    setvbuf(stdout, buffer, _IOFBF, BUFSIZ);

    assert(calc_pfix(pfix_result, "23", "12", '+'));
    x = atoi(pfix_result);
    assert(x == 35);
    assert(calc_pfix(pfix_result, "23", "12", '-'));
    x = atoi(pfix_result);
    assert(x == 11);
    assert(calc_pfix(pfix_result, "24", "12", '/'));
    x = atoi(pfix_result);
    assert(x == 2);
    assert(calc_pfix(pfix_result, "23", "12", '*'));
    x = atoi(pfix_result);
    assert(x == 276);
    assert(!calc_pfix(pfix_result, "23", "12", '>'));
    assert(strsame(buffer, 
    "Can't understand that operator: >\n"));
    buff_reset(buffer);
    assert(strsame(buffer, ""));


    turtle* t, *t_start;
    //fail var_get
    FILE* f = fopen("TTLs/ok_parse_fail_interp.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    assert(!prog(&p, &t));
    assert(strsame(buffer, 
    "Interpretor Error: Attempting to access unassigned variable occurred in var_get function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);

    //fail set_col
    f = fopen("Testing/Test_TTLs/fail_set_col1.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    assert(!prog(&p, &t));
    assert(strsame(buffer, 
    "Interpretor Error: \"ORANGE\" is not a valid colour in set_col function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);

    f = fopen("Testing/Test_TTLs/fail_set_col2.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    assert(!prog(&p, &t));
    assert(strsame(buffer, 
    "Interpretor Error: \"MAUVE\" is not a valid colour in set_col function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);

    // testing loops
    f = fopen("Testing/Test_TTLs/fail_interp_loop1.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    prog(&p, &t);
    assert(strsame(buffer, 
    "Interpretor Error: LOOP has no END occurred in skip_to_loop_end function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);

    f = fopen("Testing/Test_TTLs/fail_interp_loop2.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    prog(&p, &t);
    assert(strsame(buffer, 
    "Interpretor Error: LOOP has no END occurred in check_for_loop_end function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);

    f = fopen("Testing/Test_TTLs/fail_interp_loop3.ttl", "r");
    p = build_program(f);
    start = p; 
    t = init_turtle();
    t_start = t;
    assert(t != NULL);
    prog(&p, &t);
    assert(strsame(buffer, 
    "Interpretor Error: LOOP has no END occurred in check_for_loop_end function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    assert(turtle_free(t_start));
    fclose(f);




    //restore stdout
    freopen("NUL", "a", stdout);
    dup2(stdout_save, STDOUT_FILENO);
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);


}
