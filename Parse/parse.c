#include "parse.h"

int main (int argc, char** argv)
{
    if(argc != 2){
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE* f = fopen(argv[1], "r");
    program *p;
    p = (program*)neill_calloc(1, sizeof(program));

    while(fscanf(f, "%s", p->word) != EOF){
        p = p->next;
        p = (program*)neill_calloc(1, sizeof(program));
    }


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

// void prog(char* str)
// {
//     if(!strsame(str, "START")){
//         ERROR("No START statement"); 
//     }
// }

