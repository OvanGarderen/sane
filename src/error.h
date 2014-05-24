#pragma once
#include "types.h"

typedef enum {
  erecurse,
  eundef,
  eio,
  enuldiv,
  eargtype,
  eargnum,
  ekeyword,
  eexpansion} Etype;

struct Error {
  Error *parent;
  Etype type;
  Value * in;
  char *text;
};

Error * Error_make(Etype, Value *, const char *, ...);
Error * Error_cpy(Error *);
Error * Error_sub(Error *, Value *, const char *, ...);
void Error_destroy(Error *);

void Error_print_flat(Error *);
void Error_print(Error *);
