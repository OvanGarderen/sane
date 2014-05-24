
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "types.h"
#include "helper.h"
#include "value.h"
#include "list.h"
#include "funcdef.h"
#include "error.h"

// basic object methods
Value *
Value_read(char * input){
  assert(input);
  char first = input[0];
  char last = input[strlen(input) - 1];
  char * c;
  char * str;
  void * ptr;
  int *i = NULL;
  float * f = NULL;
  List * list;
  Value * temp;
  Value * value = malloc(sizeof(Value));
  value->type = vnull;
  value->get = NULL;

  if(!strcmp(input,"NIL") || !strcmp(input,"nil")){
    // got actual vnull value
  }
  else if (first == '"' && last == '"') {
    //    puts("Oh boy, een string.");
    str = malloc(strlen(input)-2);
    strncpy(str,input+1,strlen(input)-2);
    str[strlen(input)-2] = '\0';

    value->type = vstring;
    value->get = str;
  }
  else if (first == '\'') {
    assert(strlen(input) != 1);
    if (strlen(input) == 2) {
      //puts("Oh boy, char");
      c = malloc(1);

      value->type = vcharacter;
      value->get = c;
    }
    else if(input[1] == '(' && last == ')') {
      //puts("Literal list!");
      //puts(input+1);
      temp = Value_read(input+1);

      free(value);
      value = temp;
      value->type = vlitlist;
    }
  }
  else if (first == '(' && last == ')') {
    //puts("Got list!");
    //puts("We gaan de recursion in!");
    input[strlen(input)-1] = 0;
    list = List_read(input+1);

    value->type = vlist;
    value->get = list;
  }
  else if (!strncmp(input,"0x",2) || !strncmp(input,"0X",2)){
    assert(scanf("%p",&ptr));

    value->type = vpointer;
    value->get = ptr;
  }
  else{
    i = malloc(sizeof(int));
    f = malloc(sizeof(float));
    if (sscanf(input,"%i",i) && !isinstring('.',input) && !isinstring('e',input)){
      //puts("integer!");
      value->type = vinteger;
      value->get = i;
      free(f);
    }
    else if (sscanf(input,"%f",f)){
      //puts("float!");
      value->type = vfloat;
      value->get = f;
      free(i);
    }
    else{
      free(i);
      free(f);
      str = malloc(strlen(input)+1);
      strncpy(str,input,strlen(input));
      str[strlen(input)] = '\0';

      value->type = vsymbol;
      value->get = str;
    }
  }
  return value;
};

Value * Value_cpy(Value *src){
  if(!src) return NULL;
  Value * new = Value_read("NIL");
  new->type = src->type;
  if(src->type == vstring || src->type == vsymbol){
    new->get = malloc(strlen(src->get)+1);
    strcpy(new->get,src->get);
  }
  else if(src->type == vlist || src->type == vlitlist){
    new->get = List_cpy(src->get);
  }
  else if(src->type == vinteger || src->type == vfloat ||
          src->type == vcharacter) {
    new->get = malloc(sizeof(void*));
    switch(src->type){
    case vinteger: *((int*)new->get) = *((int*)src->get); break;
    case vfloat: *((float*)new->get) = *((float*)src->get); break;
    case vcharacter: *((char*)new->get) = *((char*)src->get); break;
    default: break;
    }
  }
  else if(src->type == vfunction){
    new->get = Funcdef_cpy(src->get);
  }
  else if(src->type == verror){
    new->get = Error_cpy(src->get);
  }
  else {
    // vpointer and vnull
    new->get = src->get;
  }
  return new;
}

void Value_destroy(Value *value){
  if(value){
    //printf("destroying %p \n",value);
    //Value_print(value);
    switch(value->type){
    case vinteger:
    case vfloat:
    case vcharacter:
    case vstring:
    case vsymbol:
      free(value->get); break;
    case vlitlist:
    case vlist:
      List_destroy(value->get); break;
    case vfunction:
      Funcdef_destroy(value->get); break;
    case verror:
      Error_destroy(value->get); break;
    default: break;
    }
    free(value);
  }
}

// printing values

void
Value_print_flat(Value * value) {
  if(!value){
    printf("None");
  }
  else{
    switch(value->type){
    case vinteger: printf("%i",*((int*) value->get)); break;
    case vfloat: printf("%f",*((float*) value->get)); break;
    case vcharacter: printf("%c",*((char*) value->get)); break;
    case vstring: printf("\"%s\"",(char*) value->get); break;
    case vpointer: printf("%p",value->get); break;
    case vlist: printf("("); List_print_flat(value->get); printf(")"); break;
    case vlitlist: printf("'("); List_print_flat(value->get); printf(")"); break;
    case vsymbol: printf(":%s",(char*) value->get); break;
    case vnull: printf("(null)"); break;
    case vfunction: Funcdef_print_flat(value->get);  break;
    case verror: Error_print_flat(value->get);  break;
    default: printf("(lel)\n");
    }
  }
}

void
Value_print(Value * value) {
  if(!value){
    printf("None\n");
  }
  else{
    prindent();
    switch(value->type){
    case vinteger: printf("%i\n",*((int*) value->get)); break;
    case vfloat: printf("%f\n",*((float*) value->get)); break;
    case vcharacter: printf("%c\n",*((char*) value->get)); break;
    case vstring: printf("\"%s\"\n",(char*) value->get); break;
    case vpointer: printf("%p\n",value->get); break;
    case vlist: printf("(\n");
      indent++;
      List_print(value->get);
      indent--;
      prindent();
      printf(")\n"); break;
    case vlitlist: printf("'(");
      List_print_flat(value->get);
      prindent();
      printf(")\n"); break;
    case vsymbol: printf(":%s\n",(char*) value->get); break;
    case vnull: printf("(null)\n"); break;
    case vfunction: Funcdef_print(value->get);  break;
    case verror: Error_print(value->get); break;
    default: printf("(nope)");
    }
  }
}

// wrapping other values
Value *
Value_wrap_int(int i) {
  Value * value = Value_read("NIL");
  value->type = vinteger;
  value->get = malloc(sizeof(int*));
  *((int*)value->get) = i;
  return value;
}

Value *
Value_wrap_List(List *list) {
  Value * value = Value_read("NIL");
  value->type = vlist;
  value->get = list;
  return value;
}

Value *
Value_wrap_Funcdef(Funcdef * func){
  Value * value = Value_read("NIL");
  value->type = vfunction;
  value->get = Funcdef_cpy(func);
  return value;
}

Value *
Value_wrap_Error(Error * error){
  Value * value = Value_read("NIL");
  value->type = verror;
  value->get = error;
  return value;
}


