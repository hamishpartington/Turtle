#include "parse.h"

int main (int argc, char** argv)
{
    if(argc != NUM_ARGS){
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(!PRODUCTION){
        test();
    }

    FILE* f = fopen(argv[1], "r");
    program *start, *p;

    start = build_program(f);
    p = start;

    bool pass = prog(&p);

    prog_free(start);

    fclose(f);

    if(pass){
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

program* build_program(FILE* f)
{
    if(!f){
        ERROR("Input NULL FILE pointer");
        return NULL;
    }
    program *start, *p;
    start = (program*)neill_calloc(1, sizeof(program));
    p = start;

    while(fscanf(f, "%s", p->word) != EOF){
        p->next = (program*)neill_calloc(1, sizeof(program));
        p = p->next;
    }
    return start;
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
    //catch programs with no end
    if((*prog)->next == NULL && !strsame((*prog)->word, "END")){
        ERROR("No end to program");
        return false;
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
    if((*prog)->word[0] == VAR){
        return var(prog);
    }else if((*prog)->word[0] == WORD){
        return word(prog);
    }else{
        ERROR("Expecting COL");
        return false;
    }
}

bool loop(program** prog)
{
    *prog = (*prog)->next;
    if(!ltr(prog, false)){
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
    
    bool pass = check_for_loop_end(*prog);
    if(!pass){
        return false;
    }

    return inlist(prog);
}

bool var(program** prog)
{
    if((*prog)->word[0] != VAR){
        ERROR("Expecting VAR");
        return false;
    }
    return ltr(prog, true);

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
    if((*prog)->word[0] == WORD){
        return word(prog);
    }else{
        return varnum(prog);
    }
}

bool varnum(program** prog)
{
    if((*prog)->word[0] == VAR){
        return var(prog);
    }else if(isdigit((*prog)->word[0]) || (*prog)->word[0] == NEGATIVE){
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
    if(!ltr(prog, false)){
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
    if((*prog)->word[0] != WORD || (*prog)->word[(len - 1)] != WORD){
        ERROR("Expecting WORD");
        return false;
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


void buff_reset(char buffer[BUFSIZ])
{
    freopen("NUL", "a", stdout);
    memset(buffer, 0, BUFSIZ);
    setvbuf(stdout, buffer, _IOFBF, BUFSIZ);
}

void test(void)
{
    program* start, *p = NULL;

    assert(!prog_free(p));
    p = (program*)neill_calloc(1, sizeof(program));

    //code to redirect stdout to buffer so error messages can be tested
    char buffer[BUFSIZ];
    fflush(stdout);
    int stdout_save = dup(STDOUT_FILENO);
    freopen("NUL", "a", stdout);
    setvbuf(stdout, buffer, _IOFBF, BUFSIZ);

    strcpy(p->word, "NOT_START");
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: No START statement occurred in prog function\n"));
    //need to reset buffer before each test
    buff_reset(buffer);
    assert(strsame(buffer, ""));

    //testing num
    assert(!num(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting NUM occurred in num function\n"));
    buff_reset(buffer);
    strcpy(p->word, "101.3");
    assert(num(&p));

    // testing var
    assert(!var(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting VAR occurred in var function\n"));
    buff_reset(buffer);
    strcpy(p->word, "$C");
    assert(var(&p));

    // testing word
    assert(!word(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting WORD occurred in word function\n"));
    buff_reset(buffer);
    strcpy(p->word, "\"$C");
    assert(!word(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting WORD occurred in word function\n"));
    buff_reset(buffer);
    strcpy(p->word, "$C\"");
    assert(!word(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting WORD occurred in word function\n"));
    buff_reset(buffer);
    strcpy(p->word, "\"$C\"");
    assert(word(&p));

    //testing isop (also tests op)
    strcpy(p->word, "-");
    assert(isop(&p));
    strcpy(p->word, "+");
    assert(isop(&p));
    strcpy(p->word, "*");
    assert(isop(&p));
    strcpy(p->word, "/");
    assert(isop(&p));
    strcpy(p->word, ".");
    assert(!isop(&p));

    assert(prog_free(p));


    //testing no start
    FILE* f = fopen("Testing/Test_TTLs/no_start.ttl", "r");
    p = build_program(f);
    start = p;

    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: No START statement occurred in prog function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing no end
    f = fopen("Testing/Test_TTLs/no_end.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: No end to program occurred in inlist function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing ins
    f = fopen("Testing/Test_TTLs/fail_ins.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting INS occurred in ins function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing col
    f = fopen("Testing/Test_TTLs/fail_col1.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting COL occurred in col function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    f = fopen("Testing/Test_TTLs/fail_col2.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting COL occurred in col function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing loop
    f = fopen("Testing/Test_TTLs/fail_loop.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting OVER occurred in loop function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing ltr
    f = fopen("Testing/Test_TTLs/fail_ltr1.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting LTR (string too long) occurred in ltr function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);
    f = fopen("Testing/Test_TTLs/fail_ltr2.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting LTR occurred in ltr function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);
    f = fopen("Testing/Test_TTLs/fail_ltr3.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting LTR occurred in ltr function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing lst
    f = fopen("Testing/Test_TTLs/fail_lst.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting { for LST occurred in lst function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing varnum
    f = fopen("Testing/Test_TTLs/fail_varnum.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting VARNUM occurred in varnum function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing set
    f = fopen("Testing/Test_TTLs/fail_set.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting SET occurred in set function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing pfix
    f = fopen("Testing/Test_TTLs/fail_pfix.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting VARNUM occurred in varnum function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    //testing item
    f = fopen("Testing/Test_TTLs/fail_item.ttl", "r");
    p = build_program(f);
    start = p;
    assert(!prog(&p));
    assert(strsame(buffer, 
    "Parsing Error: Expecting VARNUM occurred in varnum function\n"));
    buff_reset(buffer);
    assert(prog_free(start));
    fclose(f);

    assert(!build_program(NULL));

    //restore stdout
    freopen("NUL", "a", stdout);
    dup2(stdout_save, STDOUT_FILENO);
    setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
}



