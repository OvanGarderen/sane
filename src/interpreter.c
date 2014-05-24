#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "eval.h"
#include "value.h"
#include "funcdef.h"
#include "list.h"
#include "error.h"
#include "context.h"
#include "builtins.h"

int main(int argc, char** argv){
  //puts("running:");
  //puts(buffer);
  //  puts("done");
  Context *context = Context_init();
  Context_add_builtins(context);

  char buffer[3000] = "(import stdlib)";

  List * res; List *ls;
  do {
    res = List_read(buffer);
    //assert(res);
    //List_print(res);
    ls = List_map_cpy(res,eval,context);
    List_destroy(res);

    if(ls){
      List * end = ls;
      for(; end->next; end = end->next);

      if(end->value->type == verror){
	Error_print(end->value->get);
      }	
      else {
	if(end->value->type != vnull){
	  printf("result:");
	  Value_print_flat(end->value);
	  puts("");
	}
      }

      List_destroy(ls);
    }

    fflush(stdout);
    printf(">>");   
    buffer[0] = '\0';
    fgets(buffer,3000,stdin);
  } while(strcmp(buffer,"quit\n") && strcmp(buffer,""));

  Context_destroy(context);
}
