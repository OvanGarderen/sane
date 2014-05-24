#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"
#include "funcdef.h"
#include "value.h"
#include "helper.h"
#include "list.h"
#include "type.h"

#define astrcpy(dest,src) dest = malloc(strlen(src)+1); strcpy(dest,src)

//basic object interface
Funcdef *
Funcdef_make(char *name, List *args, List *body) {
  Funcdef * def = malloc(sizeof(Funcdef));
  def->name = malloc(strlen(name)+1);
  strcpy(def->name,name);
  def->ismacro = 0;
  def->numargs = List_len(args);
  def->args = calloc(sizeof(char*),abs(def->numargs));

  int i = 0;
  for(List *cur = args; cur; cur=cur->next){
    assert(cur->value);
    assert(cur->value->type == vsymbol);
    def->args[i] = malloc(strlen(cur->value->get)+1);
    strcpy(def->args[i],cur->value->get);
    i++;
    if(((char *)cur->value->get)[0] == '@' && !isallcaps((char*)cur->value->get)){
      def->numargs = -abs(def->numargs);
    }
  }

  def->types = calloc(sizeof(Vtype),abs(def->numargs));
  for(int j=0; j < abs(def->numargs); j++)
    def->types[j] = vany;

  def->body = List_cpy(body);
  def->handler = stdfunchandler;
  return def;
}

void
Funcdef_addtypes(Funcdef *def, List *types){
  int i = 0;
  for(List *cur = types; types && i<abs(def->numargs); types = types->next){
    assert(cur->value && cur->value->type == vsymbol);
    def->types[i] = Vtype_read(cur->value->get);
    i++;
  }
}

Funcdef *
Funcdef_make_macro(char *name, List *args,List *body){
  Funcdef * def = Funcdef_make(name,args,body);
  def->ismacro = 1;
  def->handler = stdmacrohandler;
  return def;
}

Funcdef *
Funcdef_cpy(Funcdef * src) {
  Funcdef * new = malloc(sizeof(Funcdef));
  new->ismacro = src->ismacro;
  new->name = malloc(strlen(src->name)+1);
  strcpy(new->name,src->name);
  new->numargs = src->numargs;
  new->types = src->types;
  new->args = calloc(sizeof(char*),abs(new->numargs));
  for(int i=0; i<abs(new->numargs); i++){
    astrcpy(new->args[i],src->args[i]);
  }
  new->body = List_cpy(src->body);
  new->handler = src->handler;
  return new;
}

void
Funcdef_destroy(Funcdef *def){
  if(def){
    free(def->name);
    for(int i=0; i<abs(def->numargs); i++){
      free(def->args[i]);
    }
    free(def->args);
    List_destroy(def->body);
    free(def);
  }
}

// printing
void
Funcdef_print_short(Funcdef * def) {
  if(!def){printf("(empty func)"); return;}

  if(!strcmp("_",def->name)){
    Funcdef_print_flat(def);
    return;
  }
  
  if(def->ismacro)
    printf("<macro ");
  else
    printf("<function ");
  printf("%s>",def->name);
}

void
Funcdef_print_flat(Funcdef * def) {
  if(!def){printf("(empty func)"); return;}
  if(def->ismacro)
    printf("<macro ");
  else{
    if(def->numargs< 0)
      printf("<variadic function ");
    else
      printf("<function ");
  }
  printf("%s ",def->name);
  if(def->numargs){
    printf("(");
    for(int i=0; i<abs(def->numargs); i++){
      printf("%s",def->args[i]);
      if(i != abs(def->numargs) - 1)
	printf(" ");
    }
    printf(")");
  }
  if(def->body){
    printf("->(");
    List_print_flat(def->body);
    printf(")");
  } 
  printf(">");
}

void
Funcdef_print(Funcdef * def) { 
  Funcdef_print_short(def);
  printf("\n");
}
