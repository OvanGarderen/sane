#include "lib.h"

Value *
addhandler(Funcdef *func, List * args, Context * context){
  Value * a = args->value;
  Value * b = args->next->value;
  Value * c = Value_read("NIL");

  if(a->type == vfloat || b->type == vfloat){
    c->type = vfloat;
    c->get = malloc(sizeof(float));
    *((float*)c->get) = *((float*)a->get) + *((float*)b->get);
  }
  else{
    c->type = vinteger;
    c->get = malloc(sizeof(int));
    *((int*)c->get) = *((int*)a->get) + *((int*)b->get);
  }
  return c;
}

Value *
subhandler(Funcdef *func, List * args, Context * context){
  Value * a = args->value;
  Value * b = args->next->value;
  Value * c = Value_read("NIL");

  if(a->type == vfloat || b->type == vfloat){
    c->type = vfloat;
    c->get = malloc(sizeof(float));
    *((float*)c->get) = *((float*)a->get) - *((float*)b->get);
  }
  else{
    c->type = vinteger;
    c->get = malloc(sizeof(int));
    *((int*)c->get) = *((int*)a->get) - *((int*)b->get);
  }
  return c;
}

Value *
multhandler(Funcdef *func, List * args, Context * context){
  Value * a = args->value;
  Value * b = args->next->value;
  Value * c = Value_read("NIL");

  if(a->type == vfloat || b->type == vfloat){
    c->type = vfloat;
    c->get = malloc(sizeof(float));
    *((float*)c->get) = *((float*)a->get) * *((float*)b->get);
  }
  else{
    c->type = vinteger;
    c->get = malloc(sizeof(int));
    *((int*)c->get) = *((int*)a->get) * *((int*)b->get);
  }
  return c;
}

Value *
divhandler(Funcdef *func, List * args, Context * context){
  Value * a = args->value;
  Value * b = args->next->value;
  Value * c = Value_read("NIL");

  if(a->type == vfloat || b->type == vfloat){
    c->type = vfloat;
    c->get = malloc(sizeof(float));
    *((float*)c->get) = *((float*)a->get) / *((float*)b->get);
  }
  else{
    if(*((int*) b->get) == 0){
      Value_destroy(c);
      return Value_Error(enuldiv,b,
			 "integer division by zero");
    }
    c->type = vinteger;
    c->get = malloc(sizeof(int));
    *((int*)c->get) = *((int*)a->get) / *((int*)b->get);
  }
  return c;
}

Value *
modhandler(Funcdef *func, List * args, Context * context){
  Value * a = args->value;
  Value * b = args->next->value;
  Value * c = Value_read("NIL");

  if(*((int*) b->get) == 0){
    Value_destroy(c);
    return Value_Error(enuldiv,b,
		       "modulo of zero");
  }

  c->type = vinteger;
  c->get = malloc(sizeof(int));

  *((int*)c->get) = *((int*)a->get) % *((int*)b->get);
  return c;
}
