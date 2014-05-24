#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "eval.h"
#include "value.h"
#include "value.h"
#include "funcdef.h"
#include "list.h"
#include "error.h"
#include "context.h"

Context *
Context_init(void){
  Context * context = malloc(sizeof(Context));
  context->parent = NULL;
  context->table = g_hash_table_new_full(g_str_hash,g_str_equal,
                                         (GDestroyNotify) Context_key_kill, (GDestroyNotify) Context_kill);
  return context;
}

Context *
Context_subcontext(Context *context){
  Context *child = Context_init();
  child->parent = context;
  return child;
}

void 
Context_destroy(Context *context){
  if(context) {
    g_hash_table_destroy(context->table);
    free(context);
  }
}

// closure
List *
Context_replace(List * ls, Context * context){
  if(!ls)
    return NULL;
  List * new = List_make(NULL,NULL);
  Value * val;
  if(ls->value && ls->value->type == vsymbol){
    // @ expansion needs to be implemented
    if(((char *)ls->value->get)[0] == '@'){
      //printf("doing %s ",(char *) ls->value->get);

      Value * reads = Value_read(((char*)ls->value->get) + 1); 
      val = eval(reads,context);
      //printf("gives value"); Value_print(val);
      if(val->type == vlist){
	Value_destroy(reads);
	List * newls = val->get;
	//List_print_flat(val->get);
	free(val);
	free(new);
	new = List_walk_tail(newls);
	new->next = Context_replace(ls->next,context);
	return newls;
      }
      Value_destroy(reads);
    }
    val = Context_get(context,ls->value->get);
    if(val)
      new->value = Value_cpy(val);
    else
      new->value = Value_cpy(ls->value);
  }
  else if(ls->value && (ls->value->type == vlist || ls->value->type == vlitlist)){
    List *lsrepl = Context_replace(ls->value->get,context);
    new->value = Value_wrap_List(lsrepl);
  }
  else {
    new->value = Value_cpy(ls->value);
  }
  new->next = Context_replace(ls->next,context);
  return new;
}

//interfacing
void 
Context_kill(gpointer data){
  //data is a Value object
  Value_destroy(data);
}

void 
Context_key_kill(gpointer data){
  free(data);
}

void 
Context_add(Context * context, char *name, Value *value){
  char *temp = malloc(strlen(name)+1);
  strcpy(temp,name);
  g_hash_table_insert(context->table,temp,Value_cpy(value));
}

void 
Context_add_upwards(Context * context, char *name, Value *value){
  Context_add(context,name,value);
  if(context->parent)
    Context_add_upwards(context->parent,name,value);
}

Value *
Context_get(Context * context, char *name){
  Value *value = g_hash_table_lookup(context->table,name);
  if(value){
    return Value_cpy(value);
  }
  else if(context->parent){
    return Context_get(context->parent,name);
  }
  return NULL;
}
