#pragma once

typedef struct Context Context;

// value
typedef struct Value Value;
typedef enum {vinteger,vfloat,vcharacter,vstring,
              vpointer,vfunction,vlist,vlitlist,vsymbol,
	      vnull, verror, vany} Vtype;

// possible values
typedef struct Error Error;
typedef struct List List;
typedef struct Funcdef Funcdef;

// function pointers
typedef Value *(*funchandler)(Funcdef *, List *, Context *);
typedef Value *(*listmapper)(Value *,Context*);
