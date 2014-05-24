#include "lib.h"
#include <string.h>

Value *
printhandler(Funcdef *func, List * args, Context * context){
  //dealing with variadic function so args[0] is a list
  args = args->value->get;
  while(args){
    if(args->value){
      if(args->value->type == vstring)
        puts(args->value->get);
      else{
        Value_print_flat(args->value);
        puts("");
      }
    }
    args = args->next;
  }
  return Value_read("NIL");
}

Value *
inputhandler(Funcdef *func, List *args, Context *context) {
  Value * prompt = args->value;

  printf("%s",(char*) prompt->get);
  char buffer[3000];
  fgets(buffer,3000,stdin);
  buffer[strlen(buffer)-1] = '\0';
  char *temp = malloc(strlen(buffer)+1);
  strcpy(temp,buffer);
  Value * val = Value_read("NIL");
  val->type = vstring;
  val->get = temp;
  return val;
}
