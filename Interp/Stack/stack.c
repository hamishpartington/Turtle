//these functions are all from Neill's github

#include "stack.h"

void* neill_calloc(int n, size_t size)
{
   void* v = calloc(n, size);
   if(v==NULL){
    fprintf(stderr, "Cannot calloc() space\n");
    exit(EXIT_FAILURE);
   }
   return v;
}

stack* stack_init(void)
{
   stack* s = (stack*) neill_calloc(1, sizeof(stack));
   return s;
}

void stack_push(stack* s, char data[MAX_PFIX])
{
   if(s){
      dataframe* f = neill_calloc(1, sizeof(dataframe));
      strcpy(f->c, data);
      f->next = s->start;
      s->start = f;
      s->size = s->size + 1;
   }
}

bool stack_pop(stack* s, char* d)
{
   if((s==NULL) || (s->start==NULL)){
      STACK_ERROR("Stack underflow due to invalid postfix expression");
      return false;
   }

   dataframe* f = s->start->next;
   strcpy(d, s->start->c);
   free(s->start);
   s->start = f;
   s->size = s->size - 1;
   return true;
}

bool stack_free(stack* s)
{
   if(s){
      dataframe* p = s->start;
      while(p!=NULL){
         dataframe* tmp = p->next;
         free(p);
         p = tmp;
      }
      free(s);
   }
   return true;
}