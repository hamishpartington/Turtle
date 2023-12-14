#include "parse.h"

int main (int argc, char** argv)
{
    if(argc != 2){
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
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
    prog(&p);


    printf("Parses OK\n");

    prog_free(start);
    

    fclose(f);

    return EXIT_SUCCESS;
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

void prog(program** prog)
{
    if(!strsame((*prog)->word, "START")){
        ERROR("No START statement");
        return; 
    }
    *prog = (*prog)->next;
    inlist(prog);
}

void inlist(program** prog)
{
    if(strsame((*prog)->word, "END")){
        return;
    }
    ins(prog);
    *prog = (*prog)->next;
    inlist(prog);
}

void ins(program** prog)
{
    if(strsame((*prog)->word, "FORWARD")){
        fwd(prog);
    }else if(strsame((*prog)->word, "RIGHT")){
        rgt(prog);
    }else if(strsame((*prog)->word, "COLOUR")){
        col(prog);
    }else if(strsame((*prog)->word, "LOOP")){
        loop(prog);
    }else if(strsame((*prog)->word, "SET")){
        set(prog);
    }else{
        ERROR("Expecting INS");
        return;
    }
}

void fwd(program** prog)
{
    *prog = (*prog)->next;
    varnum(prog);
}

void rgt(program** prog)
{
    *prog = (*prog)->next;
    varnum(prog);
}

void col(program** prog)
{
    *prog = (*prog)->next;
    if((*prog)->word[0] == '$'){
        var(prog);
    }else if((*prog)->word[0] == '"'){
        word(prog);
    }else{
        ERROR("Expecting COL");
    }
}

void loop(program** prog)
{
    *prog = (*prog)->next;
    ltr(prog);
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "OVER")){
        ERROR("Expecting OVER");
        return;
    }
    *prog = (*prog)->next;
    lst(prog);
    *prog = (*prog)->next;
    inlist(prog);
}

void var(program** prog)
{
    if((*prog)->word[0] != '$'){
        ERROR("Expecting VAR");
        return;
    }
    ltr(prog);

}

void ltr(program** prog)
{
    if((*prog)->word[0] == '$'){
        if(!isupper((*prog)->word[1])){
            ERROR("Expecting LTR");
            return;
        }
    }else{
        if(!isupper((*prog)->word[0])){
            ERROR("Expecting LTR");
            return;
        }
    }


}

void lst(program** prog)
{
    if(!strsame((*prog)->word, "{")){
        ERROR("Expecting { for LST");
        return;
    }
    *prog = (*prog)->next;
    items(prog);
}

void items(program** prog)
{
    if(strsame((*prog)->word, "}")){
        return;
    }
    item(prog);
    *prog = (*prog)->next;
    items(prog);
}

void item(program** prog)
{
    if((*prog)->word[0] == '"'){
        word(prog);
    }else{
        varnum(prog);
    }
}

void varnum(program** prog)
{
    if((*prog)->word[0] == '$'){
        var(prog);
    }else if(isdigit((*prog)->word[0]) || (*prog)->word[0] == '-'){
        num(prog);
    }else{
        ERROR("Expecting VARNUM");
        return;
    }
}

void num(program** prog)
{
    double n;

    if(sscanf((*prog)->word, "%lf", &n) != 1){
        ERROR("Expecting NUM");
        return;
    }
}

void set(program** prog)
{
    *prog = (*prog)->next;
    ltr(prog);
    *prog = (*prog)->next;
    if(!strsame((*prog)->word, "(")){
        ERROR("Expecting SET");
        return;
    }
    *prog = (*prog)->next;
    pfix(prog);
}

void pfix(program** prog)
{
    if(strsame((*prog)->word,")")){
        return;
    }else if(isop(prog)){
        op(prog);
    }else{
        varnum(prog);
    }
    *prog = (*prog)->next;
    pfix(prog);
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

void op(program** prog)
{
    if(!isop(prog)){
        ERROR("Expecting OP");
        return;
    }
}

void word(program** prog)
{
    if((*prog)->word[0] != '"'){
        ERROR("Expecting WORD");
        return;
    }
}













