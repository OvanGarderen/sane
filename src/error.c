#define _GNU_SOURCE

#include "error.h"
#include <stdlib.h>
#include <stdarg.h>

#include <stdio.h>
#include "helper.h"

#include "value.h"

Error *
Error_make(Etype type, Value *in, const char *fmt, ...){
  va_list args;
  va_start(args,fmt);

  Error * error = malloc(sizeof(Error));
  error->type = type;
  error->in = Value_cpy(in);
  error->text = NULL;
  vasprintf(&error->text, fmt, args);
  error->parent = NULL;
  return error;
}


Error *
Error_cpy(Error * src){
  Error *new = Error_make(src->type,src->in,src->text);

  if(src->parent){
    new->parent = Error_cpy(src->parent);
  }
  return new;
}

Error *
Error_sub(Error *parent, Value *in, const char *fmt, ...){
  va_list args;
  va_start(args,fmt);

  Error * error = malloc(sizeof(Error));
  error->type = erecurse;
  error->text = NULL;
  error->in = Value_cpy(in);
  vasprintf(&error->text, fmt, args);
  error->parent = parent;
  return error;
}

void
Error_destroy(Error * error){
  if(!error)
    return;
  
  Value_destroy(error->in);
  Error_destroy(error->parent);
  free(error->text);
  free(error);
}

// printing

void
Error_print_flat(Error * error){
  if(error){
    switch(error->type){
    case erecurse: Error_print_flat(error->parent); break;
    case eundef: printf("undefined error"); break;
    case eargtype: printf("argument type error"); break;
    case eargnum: printf("argument number error"); break;
    case ekeyword: printf("keyword error"); break;
    case eio: printf("I/O error"); break;
    case enuldiv: printf("division by zero error"); break;
    default: printf("some error");
    }
  }
}

void
Error_print(Error * error){
  if(error){
    Error_print(error->parent);

    printf("  !!  ");
    Error_print_flat(error);
    puts("  !! in:");
    ++indent;
    
    prindent(); 
    Value_print(error->in);
    prindent(); 
    puts(error->text);

    --indent;
  }
}
