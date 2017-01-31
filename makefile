#
# PROGRAM:    Assignment 3
# PROGRAMMER: Drew Stefanik
# LOGON ID:   z1753812
# DATE DUE:   04/22/2016
#

# Compiler variables
CCFLAGS = -ansi -Wall

all: load query

# Rule to link object code files to create executable file
load: load.o
	g++ $(CCFLAGS) -o load load.o -lpq

query: query.o
	g++ $(CCFLAGS) -o query query.o -lpq

# Rules to compile source code files to object code
load.o: load.cc
	g++ $(CCFLAGS) -c load.cc

query.o: query.cc
	g++ $(CCFLAGS) -c query.cc

# Pseudo-target to remove object code and executable files
clean:
	-rm *.o load query
