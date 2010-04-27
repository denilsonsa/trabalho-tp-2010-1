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
SRCS = fisica-teste.c fisica.c
OBJS = $(SRCS:.c=.o)

#Basic rules
.PHONY: all clean
all: $(PROGS)

clean:
	for a in $(OBJS) $(PROGS); do \
		rm -f $$a ; \
	done


fisica-teste: fisica.o fisica-teste.o
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@

#Some implicit rules
fisica.o: fisica.c fisica.h
fisica-teste.o: fisica-teste.c fisica.c fisica.h


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

