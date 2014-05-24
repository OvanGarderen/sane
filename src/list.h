#pragma once
#include "types.h"
#include <stdio.h>

struct List {
  Value * value;
  List * next;
};

List * List_make(Value *, List *);
List * List_read(char *);
List * List_read_file(FILE *);
List * List_cpy(List *);
void List_destroy(List *);

void List_print_flat(List *);
void List_print(List *);

int List_len(List *);
Value *List_get(List *, int);
List * List_walk(List *,int);
List * List_walk_tail(List *);

List *List_map(List *, listmapper, Context *);
List *List_map_cpy(List *, listmapper, Context *);

