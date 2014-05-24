#include "types.h"

// macro's
Value * defhandler(Funcdef *, List *, Context *);
Value * _funchandler(Funcdef *, List *, Context *);
Value * macrohandler(Funcdef *, List *, Context *);
Value * ifhandler(Funcdef *, List *, Context *);
Value * lethandler(Funcdef *, List *, Context *);
Value * loophandler(Funcdef *, List *, Context *);

Value * importhandler(Funcdef *, List *, Context *);
Value * externhandler(Funcdef *, List *, Context *);

void Context_add_builtins(Context *);
