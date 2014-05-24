#pragma once
#include "types.h"

struct Funcdef {
  int ismacro;
  char * name;
  int numargs;
  char** args;
  Vtype *types;
  List * body;
  funchandler handler;
};

// object methods
Funcdef * Funcdef_make(char *, List *, List *);
Funcdef * Funcdef_make_macro(char *, List *,List *);
Funcdef * Funcdef_cpy(Funcdef *);
void Funcdef_destroy(Funcdef *);
void Funcdef_addtypes(Funcdef *, List *);

// printing
void Funcdef_print_short(Funcdef *);
void Funcdef_print_flat(Funcdef *);
void Funcdef_print(Funcdef *);

// these are implemented in builtins.h
Value * stdfunchandler(Funcdef *, List *, Context *);
Value * stdmacrohandler(Funcdef *, List *, Context *);
