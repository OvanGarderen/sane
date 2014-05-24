#pragma once
#include "types.h"

struct Value {
  Vtype type;
  void * get;
};


Value * Value_read(char *);
Value * Value_cpy(Value *);
void Value_destroy(Value *);

void Value_print_flat(Value *);
void Value_print(Value *);

Value * Value_wrap_int(int);
Value * Value_wrap_List(List *);
Value * Value_wrap_Funcdef(Funcdef *);
Value * Value_wrap_Error(Error *);

#define Value_Error(type, in, fmt, ...) Value_wrap_Error(Error_make(type,in,fmt, ##__VA_ARGS__))

