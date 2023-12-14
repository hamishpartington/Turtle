#include "interp.h"

int main (int argc, char** argv)
{
    if(argc < MIN_ARGS || argc > MAX_ARGS){
        fprintf(stderr, "Usage: %s <input file> (<output_file>)", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* f = fopen(argv[1], "r");
    program *start, *p;
    start = (program*)neill_calloc(1, sizeof(program));
    p = start;

    while(fscanf(f, "%s", p->word) != EOF){
        p->next = (program*)neill_calloc(1, sizeof(program));
        p = p->next;
    }
    p = start;

    bool pass = prog(&p);

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
    *prog = (*prog)->next;
    return inlist(prog);
}

bool inlist(program** prog)
{
    if(strsame((*prog)->word, "END")){
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
    *prog = (*prog)->next;
    return varnum(prog);
}

bool rgt(program** prog)
{
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
    double n;

    if(sscanf((*prog)->word, "%lf", &n) != 1){
        ERROR("Expecting NUM");
        return false;
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













