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

LIBSRCS = lib/fisica.c lib/nbiocore.c
PROGSRCS = src/fisica-teste.c src/fisica-teste-args.c 

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
# Dependency of the main programs

fisica-teste: \
	lib/fisica.o lib/nbiocore.o \
	src/fisica-teste.o src/fisica-teste-args.o

	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@


############################################################
# Some implicit rules to describe all the dependencies

lib/fisica.o: lib/fisica.c lib/fisica.h

lib/nbiocore.o: lib/nbiocore.c lib/nbiocore.h

src/fisica-teste.o: \
	lib/fisica.h lib/nbiocore.h \
	src/fisica-teste.c src/fisica-teste-args.h

src/fisica-teste-args.o: \
	src/fisica-teste-args.c src/fisica-teste-args.h


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

