#CPPFLAGS Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers).
#CFLAGS   Extra flags to give to the C compiler.
#CXXFLAGS Extra flags to give to the C++ compiler.
#LDFLAGS  Extra flags to give to compilers when they are supposed to invoke the linker, 'ld'.

#Standard variables:
CC  = gcc
CXX = g++
CFLAGS   = -Wall $(DEBUGFLAGS)
CXXFLAGS = $(CFLAGS)
LDFLAGS  = -lm

#DEBUGFLAGS = -g -ggdb

#=====================================#
#  No need to change below this line  #
#=====================================#

PROGS = client-udp server-udp
OBJECTS = client-udp.o server-udp.o

#Basic rules
.PHONY: all clean
all: $(PROGS)

clean:
	for a in $(OBJECTS) $(PROGS); do \
		rm -f $$a ; \
	done


#Some implicit rules
client-udp:   client-udp.o
client-udp.o: client-udp.c
server-udp:   server-udp.o
server-udp.o: server-udp.c


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

