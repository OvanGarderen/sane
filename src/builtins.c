#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "builtins.h"
#include "eval.h"
#include "error.h"
#include "helper.h"
#include "value.h"
#include "list.h"
#include "context.h"
#include "funcdef.h"

Vtype Tstring[] = {vstring};

/* char *Fprintargs[] = {"@args"}; */
/* Funcdef Fprint = {0,"print",-1,Fprintargs,NULL,NULL,printhandler}; */
/* char *Finputargs[] = {"prompt"}; */
/* Funcdef Finput = {0,"input",1,Finputargs,Tstring,NULL,inputhandler}; */

Vtype Tlist[] = {vlist};
//lists
/* char *Fheadargs[] = {"ls"}; */
/* Funcdef Fhead = {0,"head",1,Fheadargs,Tlist,NULL,headhandler}; */
/* char *Ftailargs[] = {"ls"}; */
/* Funcdef Ftail = {0,"tail",1,Ftailargs,Tlist,NULL,tailhandler}; */
/* char *Flistargs[] = {"@args"}; */
/* Funcdef Flist = {0,"list",-1,Flistargs,NULL,NULL,listhandler}; */
/* char *Femptyargs[] = {"ls"}; */
/* Funcdef Fempty = {0,"empty",1,Femptyargs,Tlist,NULL,emptyhandler}; */

/* //arithmatic */
/* Vtype Tintegers[] = {vinteger,vinteger}; */
/* char *Faddargs[] = {"a","b"}; */
/* Funcdef Fadd = {0,"+",2,Faddargs,Tintegers,NULL,addhandler}; */
/* char *Fsubargs[] = {"a","b"}; */
/* Funcdef Fsub = {0,"-",2,Fsubargs,Tintegers,NULL,subhandler}; */
/* char *Fmultargs[] = {"a","b"}; */
/* Funcdef Fmult = {0,"*",2,Fmultargs,Tintegers,NULL,multhandler}; */
/* char *Fdivargs[] = {"a","b"}; */
/* Funcdef Fdiv = {0,"/",2,Fdivargs,Tintegers,NULL,divhandler}; */
/* char *Fmodargs[] = {"a","b"}; */
/* Funcdef Fmod = {0,"%",2,Fmodargs,Tintegers,NULL,modhandler}; */

//macro's
char *Fdefargs[] = {"name","args","@body"};
Funcdef Fdef = {1,"def",-3,Fdefargs,NULL,NULL,defhandler};
char *Ffuncargs[] = {"args","body"};
Funcdef Ffunc = {1,"func",2,Ffuncargs,NULL,NULL,_funchandler};
char *Fmacroargs[] = {"name","args","body"};
Funcdef Fmacro = {1,"macro",3,Fmacroargs,NULL,NULL,macrohandler};

char *Fifargs[] = {"condition","@THEN","first","@ELSE","second"};
Funcdef Fif = {1,"if",5,Fifargs,NULL,NULL,ifhandler};
char *Fletargs[] = {"variable","value","@IN","body"};
Funcdef Flet = {1,"let",4,Fletargs,NULL,NULL,lethandler};
char *Floopargs[] = {"var","start","end","@IN","body"};
Funcdef Floop = {1,"loop",5,Floopargs,NULL,NULL,loophandler};

char *Fimportargs[] = {"import"};
Funcdef Fimport = {1,"import",1,Fimportargs,NULL,NULL,importhandler};
char *Fexternargs[] = {"func","@FROM","file"};
Funcdef Fextern = {1,"extern",3,Fexternargs,NULL,NULL,externhandler};

Funcdef *builtins[] = {
  &Fdef, &Ffunc, &Fmacro,
  &Fif, &Flet, &Floop, 
  &Fimport, &Fextern,
  NULL
};

void Context_add_builtins(Context *context){
  for(Funcdef **cur=builtins; *cur; cur++){
    Value * wrap = Value_wrap_Funcdef(*cur);
    Context_add(context,(*cur)->name,wrap);
    Value_destroy(wrap);
  }
}

// implementation
Value *
stdfunchandler(Funcdef *func, List *args, Context *context){
  context = Context_subcontext(context);
  if(func->numargs){
    for(int i=0; i<abs(func->numargs); i++){
      if(func->args[i][0] == '@')
	Context_add(context,func->args[i]+1,List_get(args,i));	
      else
	Context_add(context,func->args[i],List_get(args,i));
      //printf("arg %i : %s = ",i,func->args[i]);
      //Value_print(List_get(args,i));
    }
  }
  Value * result;
  if(func->body){
    result = eval(Value_wrap_List(func->body),context);
  }
  else {
    result = Value_Error(eundef,Value_wrap_Funcdef(func),
				 "function has an empty body!!");
  }
  Context_destroy(context);
  return result;
}

Value *
stdmacrohandler(Funcdef *func, List *args, Context *context){
  /* Moet een nieuwe implementatie krijgen */
  /* met een recursive_replace functie */
  Context *repl_dict = Context_init();
  if(func->numargs) {
    for(int i=0; i<abs(func->numargs); i++){
      Context_add(repl_dict,func->args[i],List_get(args,i));
      //printf("arg %i : %s = ",i,func->args[i]);
      //Value_print(List_get(args,i));
    }
  }
  List *newbody = Context_replace(func->body,repl_dict);
  Context_destroy(repl_dict);
  Value *valnewbody = Value_wrap_List(newbody);
  Value * result = eval(valnewbody,context);
  Value_destroy(valnewbody);
  return result;
}


/* Handling Lists */

/* Handling arithmatic */


/* various macro's */

Value *
defhandler(Funcdef *func, List *args, Context *context){
  if(List_len(args->next->next->value->get) > 1){
    printf("To many arguments to macro def\n");
    return NULL;
  }
  else if(List_len(args->next->next->value->get) == 0){
    Value * name = List_get(args,0);
    Value * value = List_get(args,1);
    if(!name || !value || name->type != vsymbol){
      puts("error in parsing def");
      return NULL;
    }
    Value * result = eval(value,context);
    Context_add_upwards(context,name->get,result);
    return result;
  }
  else {
    Value * name = List_get(args,0);
    Value * fargs = List_get(args,1);
    Value * body = List_get((List*) List_get(args,2)->get,0);

    //printf("making func %s\n",name->get);
    //Value_print(fargs);
    //Value_print(body);
    //puts("closing body");
    Context *closure = Context_subcontext(context);
    for(List *cur = fargs->get; cur; cur = cur->next){
      assert(cur->value->type == vsymbol);
      //puts(cur->value->get);
      Context_add(closure,cur->value->get,cur->value);
    }
    List *closedbody = Context_replace(body->get,closure);
    //List_print(closedbody);

    Funcdef * f = Funcdef_make(name->get,fargs->get,closedbody);
    Value * wrapper = Value_wrap_Funcdef(f);
    Context_add_upwards(context,name->get,wrapper);
    List_destroy(closedbody);
    Context_destroy(closure);
    //    Value_print(wrapper);
    return wrapper;
  }
}

Value *
_funchandler(Funcdef *func, List *args, Context *context){
  Value * fargs = args->value;
  Value * body = args->next->value;
  if(!fargs || !body || fargs->type != vlist){
    puts("error in parsing func");
    return NULL;
  }

  Context *closure = Context_subcontext(context);
  Value_print(Context_get(context,"args"));
  for(List *cur = fargs->get; cur; cur = cur->next){
    assert(cur->value->type == vsymbol);
    printf("%s",(char*)cur->value->get);
    Context_add(closure,cur->value->get,cur->value);
  }

  List *closedbody = Context_replace(body->get,closure);
  Funcdef * f = Funcdef_make("_",fargs->get,closedbody);

  Value * wrapper = Value_wrap_Funcdef(f);
  List_destroy(closedbody);
  Context_destroy(closure);
  return wrapper;
}

Value *
macrohandler(Funcdef *func, List *args, Context *context){
  Value * name = args->value;
  Value * fargs = args->next->value;
  Value * body = args->next->next->value;
  if(!name || !fargs || !body || name->type != vsymbol || fargs->type != vlist){
    puts("error in parsing def");
    return NULL;
  }
  Funcdef * f = Funcdef_make_macro(name->get,fargs->get,body->get);
  Value * wrapper = Value_read("NIL");
  wrapper->type = vfunction;
  wrapper->get = f;
  Context_add(context,name->get,wrapper);
  return wrapper;
}

Value *
ifhandler(Funcdef *func, List *args, Context *context){
  Value * condition = args->value;
  Value * first     = args->next->next->value;
  Value * second    = args->next->next->next->next->value;
  Value * ret = eval(condition,context);
  assert(ret);
  if(ret->type != vinteger){
    printf("if condition does not return an integer, in:");
    Value_print(condition);
    return Value_read("NIL");
  }
  int boolean = *((int*)ret->get);
  if(boolean)
    return eval(first,context);
  return eval(second,context);
}

Value *
lethandler(Funcdef *func, List *args, Context *context){
  Value * varname = args->value;
  Value * value = args->next->value;
  Value * body = args->next->next->next->value;
  assert(varname && value && body && varname->type == vsymbol);
  context = Context_subcontext(context);
  Context_add(context,varname->get,eval(value,context));
  Value * result = eval(body,context);
  Context_destroy(context);
  return result;
}

Value *
loophandler(Funcdef *func, List *args, Context *context) {
  Value * var    = args->value;
  Value * start  = args->next->value;
  Value * end    = args->next->next->value;
  Value * body   = args->next->next->next->next->value;
  assert(var && start && end && body);
  assert(var->type == vsymbol && start->type == vinteger
         &&   end->type == vinteger);
  context = Context_subcontext(context);
  Value * result = Value_read("NIL");
  int istart = *((int*)start->get);
  int iend   = *((int*)end->get);
  int i;
  for(i=istart; i<iend; i++){
    Value * cur = Value_wrap_int(i);
    Context_add(context,var->get,cur);
    Value_destroy(cur);
    Value_destroy(result);
    result = eval(body,context);
  }
  Context_destroy(context);
  //Value_print(result);
  return result;
}


// extended functionality 

Value *
importhandler(Funcdef *func, List *args, Context *context) {
  Value * name = args->value;
  assert(name && name->type == vsymbol);

  char *buffer = malloc(strlen(name->get)+4);
  strcpy(buffer,(char *)name->get);
  strcpy(buffer+strlen(name->get),".tx");
  printf("importing %s\n",buffer);
  fflush(stdout);

  FILE *fp;
  fp = fopen(buffer,"r");
  List * ls = List_read_file(fp);
  fclose(fp);
  free(buffer);
  List * cpy = List_map_cpy(ls,eval,context);
  List_destroy(cpy);
  List_destroy(ls);
  return Value_read("NIL");
}

Value *
externhandler(Funcdef *func, List *args, Context *context) {
  Value * fname = args->value;
  Value * file  = args->next->next->value;

  assert(fname && file && 
	 (fname->type == vstring || fname->type == vsymbol)
	 && (file->type == vstring || file->type == vsymbol));

  if(file->type == vsymbol){
    char *buffer = malloc(strlen(file->get)+7);
    strcpy(buffer,(char *)file->get);
    strcpy(buffer+strlen(file->get),".btx");
    Value_destroy(file);
    file = Value_read("NIL");
    file->get = buffer;
    file->type = vstring;
  }

  printf("linking %s\n",(char*)file->get);

  FILE * fp = fopen(file->get,"r");
  if(!fp)
    return Value_Error(eio,file,"no such file");
  List * ls = List_read_file(fp);
  fclose(fp);
  
  Value * newfunc = NULL;

  while(ls) {
    Value * curlist = ls->value;
    assert(curlist->type == vlist);
    assert(curlist->get);

    List * cls = curlist->get;
    Value * tname = cls->value;
    assert(tname->type == vsymbol);

    if(!strcmp(tname->get,fname->get) || !strcmp(fname->get,"all")){
      assert(List_len(cls) == 6);
      Value * filefrom    = cls->next->value;
      Value * nameinfile  = cls->next->next->value;
      Value * argspec     = cls->next->next->next->value;
      Value * typespec    = cls->next->next->next->next->value;
      Value * ismacro     = cls->next->next->next->next->next->value;

      if(!(filefrom->type == vstring &&
	   (nameinfile->type == vstring || nameinfile->type == vsymbol) &&
	   argspec->type == vlist &&
	   typespec->type == vlist &&
	   ismacro->type == vinteger)){
	newfunc = Value_Error(eargtype,Value_wrap_List(cls),
			      "wrong argument types");
	break;
      }

      void *handle = dlopen(filefrom->get,RTLD_LAZY);
      if(!handle){
	newfunc = Value_Error(eio,filefrom,
			      "no such file or not a shared object file");
	break;
      }
      void * dlfunc = dlsym(handle,nameinfile->get);
      if(!dlfunc){
	newfunc = Value_Error(eio,nameinfile,
			      "symbol not found in file %s",filefrom->get);
	break;
      }

      Funcdef * new = Funcdef_make(tname->get, argspec->get, NULL);
      new->ismacro = *((int*)ismacro->get);
      Funcdef_addtypes(new,typespec->get);

      new->handler = dlfunc;
      assert(new->handler);

      newfunc = Value_wrap_Funcdef(new);
      Context_add_upwards(context,tname->get,newfunc);

      if(strcmp(fname->get,"all"))
	break;
    }
    ls = ls->next;
  }
  List_destroy(ls);

  if(!newfunc){
    return Value_Error(eundef,fname,"no such function found in file %s",
		       (char*)file->get);
  }
  return newfunc;
}
