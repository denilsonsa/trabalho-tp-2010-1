#CPPFLAGS Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers).
#CFLAGS   Extra flags to give to the C compiler.
#CXXFLAGS Extra flags to give to the C++ compiler.
#LDFLAGS  Extra flags to give to compilers when they are supposed to invoke the linker, 'ld'.

#Standard variables:
CC  = gcc
CXX = g++
CFLAGS   = -Wall -pipe $(DEBUGFLAGS)
CXXFLAGS = $(CFLAGS)
LDFLAGS  =
LIBS = -lm

#DEBUGFLAGS = -g -ggdb

#=====================================#
#  No need to change below this line  #
#=====================================#

PROGS = fisica-teste
SRCS = fisica.c fisica-teste.c fisica-teste-args.c nbiocore.c
OBJS = $(SRCS:.c=.o)

#Basic rules
.PHONY: all clean
all: $(PROGS)

clean:
	rm -f $(OBJS) $(PROGS)


fisica-teste: $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

#Some implicit rules
fisica.o: fisica.c fisica.h
fisica-teste.o: fisica-teste.c fisica.h fisica-teste-args.h nbiocore.h
fisica-teste-args.o: fisica-teste-args.c fisica-teste-args.h
nbiocore.o: nbiocore.c nbiocore.h


#Some header dependency
#main.o: common.h tileset.h
#tileset.o: tileset.h


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

