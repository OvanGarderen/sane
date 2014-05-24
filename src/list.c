#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "list.h"
#include "value.h"

// basic object functions
List *
List_make(Value * value, List * next) {
  List * list = malloc(sizeof(List));
  list->value = value;
  list->next = next;
  return list;
};

List *
List_read(char * input){
  /* puts("parsing ("); */
  /* puts(input); */
  /* puts(")"); */
  assert(input);
  List *start = List_make(NULL,NULL); //only a placeholder
  List *cur = start;
  char buffer[1000] = "";
  char *bufferp = buffer;
  int splittan = 0;
  int braces = 0;
  int quote = 0;
  int escape = 0;

  for(char *c=input; 1; c++){
    List * newl;
    //printf("%c  : ()%i,\"%i,\\%i,' '%i\n",*c,braces,quote,escape,splittan);
    if ((isspace(*c) || *c == '\0') && splittan &&
        !braces && !quote && !escape) {
      //      printf("-----------   %s\n",buffer);
      newl = List_make(Value_read(buffer),NULL);
      cur->next = newl;
      cur = newl;
      splittan = 0;
      if (*c == '\0'){
        break;
      }
    }
    else if (*c == '\0'){
      break;
    }
    else if (!isspace(*c) || quote || braces || escape){
      if (!splittan) {
        splittan = 1;
        bufferp = buffer;
      }

      if(escape){
        switch(*c){
        case 't': *c = '\t'; break;
        case 'n': *c = '\n'; break;
        case 'v': *c = '\v'; break;
        case 'f': *c = '\f'; break;
        case 'r': *c = '\r'; break;
        case 'b': *c = '\b'; break;
        }
        escape = 0;

        *bufferp = *c;
        bufferp++;
        *bufferp = '\0';
      }
      else if(*c == '\\'){
        escape = 1;
      }
      else {

        if(!quote){
          if(*c == '(')
            braces++;
          if(*c == ')'){
            assert(braces > 0);
            braces--;
          }
        }
        if(*c == '"')
          quote = ~quote;

        *bufferp = *c;
        bufferp++;
        *bufferp = '\0';
      }
    }
  }
  assert(braces == 0);
  assert(quote == 0);
  assert(escape == 0);
  assert(splittan == 0);
  cur = start->next;
  free(start);
  return cur;
}

List *
List_read_file(FILE *fp){
  assert(fp);
  List *start = List_make(NULL,NULL); //only a placeholder
  List *cur = start;
  char buffer[10000] = "";
  char *bufferp = buffer;
  int splittan = 0;
  int braces = 0;
  int quote = 0;
  int escape = 0;
  int comment = 0;

  for(char c=fgetc(fp); 1; c=fgetc(fp)){
    List * newl;
    if(c == '\n' && comment)
      comment = 0;
    else if(!quote && c == ';')
      comment = 1;

    if(comment){
      //do nothing
    }
    else if ((isspace(c) || c == '\0' || c == EOF) && splittan &&
        !braces && !quote && !escape) {
      
      //printf("-----------   %s\n",buffer);
      newl = List_make(Value_read(buffer),NULL);
      cur->next = newl;
      cur = newl;
      splittan = 0;
      if (c == '\0' || c == EOF){
        break;
      }
    }
    else if (c == '\0' || c == EOF){
      break;
    }
    else if (!isspace(c) || quote || braces || escape){
      if (!splittan) {
        splittan = 1;
        bufferp = buffer;
      }

      if(escape){
        switch(c){
        case 't': c = '\t'; break;
        case 'n': c = '\n'; break;
        case 'v': c = '\v'; break;
        case 'f': c = '\f'; break;
        case 'r': c = '\r'; break;
        case 'b': c = '\b'; break;
        }
        escape = 0;

        *bufferp = c;
        bufferp++;
        *bufferp = '\0';
      }
      else if(c == '\\'){
        escape = 1;
      }
      else {

        if(!quote){
          if(c == '(')
            braces++;
          if(c == ')'){
            assert(braces > 0);
            braces--;
          }
        }
        if(c == '"')
          quote = ~quote;

        *bufferp = c;
        bufferp++;
        *bufferp = '\0';
      }
    }
  }
  assert(braces == 0);
  assert(quote == 0);
  assert(escape == 0);
  assert(splittan == 0);
  cur = start->next;
  free(start);
  return cur;
}

List *
List_cpy(List *src) {
  if(!src)
    return NULL;
  List * ls = List_make(NULL,NULL);
  ls->value = Value_cpy(src->value);
  ls->next = List_cpy(src->next);
  return ls;
}

void
List_destroy(List * list) {
  if(list) {
    //printf("destroying ");
    //Value_print(list->value);
    Value_destroy(list->value);
    List_destroy(list->next);
    free(list);
  }
};


// printing
void
List_print_flat(List * list) {
  if(list){
    Value_print_flat(list->value);
    if(list->next)
      printf(" ");
    List_print_flat(list->next);
  }
};

void
List_print(List * list) {
  if(list){
    Value_print(list->value);
    List_print(list->next);
  }
};

// interfacing with list
int
List_len(List *ls) {
  if(!ls)
    return 0;
  return 1 + List_len(ls->next);
}

Value *
List_get(List *start, int entry){
  if(!start)
    return Value_read("NIL");
  if(entry < 0)
    entry = entry % List_len(start);
  if(entry == 0)
    return start->value;
  return List_get(start->next,entry-1);
}

List *
List_walk(List *ls, int entry){
  if(!ls) return NULL;

  if(entry < 0)
    entry = entry % List_len(ls);
  if(entry == 0)
    return ls;
  return List_walk(ls->next,entry-1);
}

List *
List_walk_tail(List *ls){
  if(!ls) return NULL;

  while(ls->next){
    ls = ls->next;
  }
  return ls;
}

List *
List_map(List * ls, listmapper map, Context * context){
  for(List *cur=ls; cur; cur = cur->next){
    Value *t = map(cur->value,context);
    cur->value = t;
  }
  return ls;
}

List *
List_map_cpy(List *src, listmapper map, Context * context){
  List * ls = List_make(NULL,NULL);
  List *lscpy = ls;
  for(List *cur=src; cur; cur = cur->next){
    Value *cpy = Value_cpy(cur->value);
    //printf("mapping function %p to ",map);
    //Value_print_flat(cpy); puts("");
    Value *t = map(cpy,context);
    if(cpy != t){
      // if the function throws out the Value then we dismiss the copy
      //printf("doin it %p ",cpy);
      //Value_print_flat(cpy);
      //puts("");
      Value_destroy(cpy);
    }
    List * newls = List_make(t,NULL);
    lscpy->next = newls;
    lscpy = newls;
  }
  lscpy = ls->next;
  free(ls);
  return lscpy;
}

