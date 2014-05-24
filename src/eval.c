#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "helper.h"
#include "types.h"
#include "eval.h"
#include "value.h"
#include "funcdef.h"
#include "list.h"
#include "error.h"
#include "context.h"

Value * eval(Value *value, Context *context){
  if(!value)
    return Value_Error(eundef,value,"trying to eval NULL");
  if(value->type != vlist){
    Value * result;
    if(value->type == vsymbol){
      Value *lookup = Context_get(context,value->get);
      if(lookup)
        return lookup;
      else
	return Value_Error(eundef,value,"symbol %s is undefined",value->get);
    }
    else if(value->type == vlitlist){
      result = Value_cpy(value);   
      result->type = vlist;
    }
    else{
      result = Value_cpy(value);
    }
   return result;
  }

  Funcdef *f = NULL;
  List *ls = (List *) value->get;
  assert(ls);
  Value *func = eval(ls->value,context);
  if(!func){
    if(ls->value && ls->value->type == vsymbol)
      return Value_Error(eundef,value,"no such function %s",ls->value->get);
    return Value_Error(eundef,value,"no such function");
  }

  if(func->type == vfunction){
    f = func->get;
  }
  else{
    //List_print_flat(ls); puts("");
    //Value_print_flat(ls->value); puts("");
    //Value_print_flat(func);
    return Value_Error(eundef,func,"expression does not evaluate to a function");
  }
  assert(f);
  List *args = ls->next;

  // check whether keywords are satisfied
  for(int i=0; i<abs(f->numargs); i++){
    if(f->args[i] && f->args[i][0] == '@'){
      if(isallcaps(f->args[i]+1)){
        char * temp = malloc(strlen(f->args[i]+1)+1);
        strcpy(temp,f->args[i]+1);
        strtolower(temp);
        Value *val = List_get(args,i);
        if(val->type != vsymbol || strcmp(temp,val->get)) {
	  return Value_Error(ekeyword,val,"expecting keyword");
	}
	free(temp);
      }
      else if(i != abs(f->numargs) - 1){
	return Value_Error(eexpansion,value,"variadic argument %s should be last argument of %s",f->args[i],f->name);
      }
    }
  }

  if(!f->ismacro){
    //args = List_map_cpy(args,eval,context);
    List *newargs = List_make(NULL,NULL);
    List *newcur = newargs;
    Vtype *types = f->types;
    for(List *cur = args; cur; cur = cur->next){
      Value * val;
      if(cur->value && cur->value->type == vsymbol && ((char *)cur->value->get)[0] == '@'){
	//printf("doing %s",(char *) cur->value->get);
   	Value * reads = Value_read(((char*)cur->value->get) + 1);
	//printf("gives value"); Value_print(reads);
	val = eval(reads,context);
	Value_destroy(reads);
	if(val->type != vlist){
	  return Value_Error(eexpansion,val,"@ expansion expects list");
	}
	List * newls = val->get;
	free(val);
	newcur->next = newls;
	newcur = List_walk_tail(newls);
      }
      else {
	val = eval(cur->value,context);
	if(val->type == verror){
	  return Value_Error(erecurse,value,"eval returns error");
	}
	if(types && val->type != *types && *types != vany){
	  return Value_Error(eargtype,val,"wrong argument to function %s",f->name);
	}
	newcur->next = List_make(val,NULL);
	newcur = newcur->next;
      }
      if(types)
	++types;
    }
    newcur = newargs->next;
    free(newargs);
    args = newcur;
  }

  //Funcdef_print_flat(f);
  //printf(" :: %i :: (",f->numargs);
  //List_print_flat(args);
  //printf(")\n");

  // check if number of arguments at least matches
  if(f->numargs < 0){
    //printf("%s : %i >= %i\n",f->name,List_len(args),-(f->numargs+1));
    if(!(List_len(args) >= -(f->numargs+1))){
      return Value_Error(eargnum,value,"variadic function %s expects at least %i arguments, got %i",f->name,abs(f->numargs)-1,List_len(args));
    }
  }
  else{
    //printf("%s : %i == %i\n",f->name,List_len(args),f->numargs);
    if(!(List_len(args) == f->numargs)){
      return Value_Error(eargnum,value,"function %s expects %i arguments, got %i",f->name,abs(f->numargs),List_len(args));
    }
  }


  if(f->numargs < 0){
    List * rem = List_walk(args,abs(f->numargs)-1);
    Value * variadic = Value_wrap_List(rem);
    if(abs(f->numargs) >= 2){
      List *args_tail = List_walk(args,abs(f->numargs)-2);
      args_tail->next = List_make(variadic,NULL);
    }
    else {
      args = List_make(variadic,NULL);
    }
  }

  Value * result = f->handler(f,args,context);
  if(!f->ismacro)    
    List_destroy(args);
  Value_destroy(func);
  return result;
}
