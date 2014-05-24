#include "lib.h"

Value *
listhandler(Funcdef *func, List *args, Context *context){
  List * ls = args->value->get;
  return Value_wrap_List(List_cpy(ls));
}

Value *
headhandler(Funcdef *func, List *args, Context *context){
  Value * a = args->value;
  List *ls = a->get;
  return Value_cpy(ls->value);
}

Value *
tailhandler(Funcdef *func, List *args, Context *context){
  Value * a = args->value;
  List *ls = ((List *) a->get)->next;
  return Value_wrap_List(List_cpy(ls));
}

Value *
emptyhandler(Funcdef *func, List *args, Context *context){
  Value * a = args->value;
  if(List_len(a->get) == 0)
    return Value_read("1");
  return Value_read("0");
}
