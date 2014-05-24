#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "helper.h"

extern int indent;

int abs(int a){
  if(a>=0){
    return a;
  }
  else {
    return -a;
  }
}

int isallcaps(char * str);

int isinstring(char c, char* string){
  for(char *k=string; *k; k++){
    if(*k == c)
      return 1;
  }
  return 0;
}

void prindent(void){
  for(int i=0; i<indent; i++)
    printf(" ");
}

void
strtoupper(char * str){
  for(; *str; str++) *str = toupper(*str);
}

void
strtolower(char * str){
  for(; *str; str++) *str = tolower(*str);
}

int
isallcaps(char * str){
  if(!str)
    return 0;
  char *temp = malloc(strlen(str)+1);
  strcpy(temp,str);
  strtoupper(temp);
  int succes = !strcmp(temp,str);
  free(temp);
  return succes;
}
