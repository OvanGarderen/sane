#include "type.h"
#include <string.h>

#define streq(A,B) (!strcmp(A,B))

Vtype
Vtype_read(char* name){
  if(streq(name,":any")){
    return vany;
  }
  else if(streq(name,":int") || streq(name,":integer")){
    return vinteger;
  }
  else if(streq(name,":float") || streq(name,":decimal")){
    return vfloat;
  }
  else if(streq(name,":char") || streq(name,":character")){
    return vcharacter;
  }
  else if(streq(name,":pointer")){
    return vpointer;
  }
  else if(streq(name,":function")){
    return vfunction;
  }
  else if(streq(name,":list")){
    return vlist;
  }
  else if(streq(name,":string")){
    return vstring;
  }
  else if(streq(name,":symbol")){
    return vsymbol;
  }
  return vany;
}
