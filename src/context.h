#pragma once
#include "types.h"
#include <glib.h>

struct Context {
  Context * parent;
  GHashTable * table;
};

Context * Context_init(void);
Context *Context_subcontext(Context *);
void Context_destroy(Context *);

List * Context_replace(List *, Context *);

//interfacing
void Context_kill(gpointer);
void Context_key_kill(gpointer);
void Context_add(Context *, char *, Value *);
void Context_add_upwards(Context *, char *, Value *);
Value * Context_get(Context *, char *);
