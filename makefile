CC :=gcc
DBG :=-g
CFLAGS := -c -O2 -Wall -Wno-variadic-macros -Wno-unused-parameter -Werror=shadow -Wextra -std=gnu11 -Wl,-export-dynamic
INCLUDE := -I./src/ $(shell pkg-config --cflags glib-2.0)
LDFLAGS := -lm $(shell pkg-config --libs glib-2.0) -ldl -rdynamic

SOURCES := $(wildcard src/*.c)
_SOURCES := $(patsubst src/%, %, $(SOURCES)) 
OBJECTS := $(patsubst %.c, objects/%.o, $(_SOURCES))
EXECUTABLE=sane

LIBSOURCES := $(wildcard src/libsrc/*.c)
_LIBSOURCES := $(patsubst src/libsrc/%, %, $(LIBSOURCES))
SHAREDOBJS := $(patsubst %.c, libs/lib%.so, $(_LIBSOURCES))

.DEPEND: $(SOURCES) $(wildcard src/*.h) $(LIBSOURCES)

all: $(EXECUTABLE) libs

libs: $(SHAREDOBJS)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

objects/libobjs/%.o: src/libsrc/%.c
	$(CC) $(CFLAGS) $(DBG) -fPIC $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[32m----------------------------------------------------------\033[0m"

objects/%.o: src/%.c 
	$(CC) $(CFLAGS) $(DBG) $(INCLUDE) $< -o $@ $(LDFLAGS)
	@echo -e "\033[31m----------------------------------------------------------\033[0m"

libs/lib%.so: objects/libobjs/%.o 
	$(CC) -shared -Wl,-soname,$@ $< -o $@ 

run:	all
	./$(EXECUTABLE)

.dummy:
	@echo "wat"

clean:
	rm objects/*.o

cleanlibs:
	rm libs/*.so
	rm objects/libobjs/*.o

restart: clean 
	rm $(EXECUTABLE)
