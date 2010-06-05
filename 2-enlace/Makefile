#CPPFLAGS Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers).
#CFLAGS   Extra flags to give to the C compiler.
#CXXFLAGS Extra flags to give to the C++ compiler.
#LDFLAGS  Extra flags to give to compilers when they are supposed to invoke the linker, 'ld'.

#Standard variables:
CC  = gcc
CXX = g++
CFLAGS   = -Wall -pipe -I./lib/ $(DEBUGFLAGS)
CXXFLAGS = $(CFLAGS)
LDFLAGS  =
LIBS = -lm

#DEBUGFLAGS = -g -ggdb

#=====================================#
#  No need to change below this line  #
#=====================================#

PROGS = fisica-teste

LIBSRCS = lib/fisica.c lib/nbiocore.c lib/terminal.c lib/util.c
PROGSRCS = src/fisica-teste.c

LIBOBJS = $(LIBSRCS:.c=.o)
PROGOBJS = $(PROGSRCS:.c=.o)
OBJS = $(LIBOBJS) $(PROGOBJS)

############################################################
# Basic rules

.PHONY: all clean
all: $(PROGS)

clean:
	rm -f $(OBJS) $(PROGS)

############################################################
# Dependencies and implicit rules...

lib/fisica.o: lib/fisica.c lib/fisica.h \
	lib/util.h

lib/nbiocore.o: lib/nbiocore.c lib/nbiocore.h

lib/util.o: lib/util.c lib/util.h


src/fisica-teste.o: src/fisica-teste.c \
	lib/fisica.h lib/nbiocore.h lib/terminal.h lib/util.h

fisica-teste: src/fisica-teste.o \
	lib/fisica.o lib/nbiocore.o lib/terminal.o lib/util.o

	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

# Done!


#The variables:
# $@ - The name of the target of the rule.
# $? - The names of all the prerequisites that are newer than the target.
# $< - The name of the first prerequisite.
# $^ - The names of all the prerequisites.

#Pattern-rules
#%.o: %.c
#	$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<
#%.o: %.cpp
#	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o $@ $<
#%.o: %.h

