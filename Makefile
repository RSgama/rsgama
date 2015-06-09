#		Makefile - Compile a set of code in C
#
#This Makefile compiles C codes based on file organization described below:
#	- inc: Folder with all headers.
#	- src: Folder with implementations of headers.
#	- bin: The binary file will be put on this folder.
#	- obj: Objects file of project. 
#Basically this Makefile first generate all the object files and next generate
#the binary file. This Makefile has a target for compile unitary tests.

# Initialize the variables used

#Folders
PACKBIN		= bin
PACKINC		= inc
PACKSRC		= src	
PACKOBJ		= obj
#Flags
CC		= gcc
LPTHREAD	= -lpthread
CUNIT		= -lcunit
WARN		= -Wall
INC		= -I./$(PACKINC)
#Create one list with all sources
CSOURCES	= $(wildcard src/*c)

# TRAGETS
all: prepare rsgama.bin

# Main target, whose the objective is generate binary files. 
# Dependencies are inserted by: $(CSOURCES:src/%.c=obj/%.o)
#	*Basically capture all the elements of list and change the pieces
#	 with "src/%.c" by "obj/%.o".
rsgama.bin: $(CSOURCES:src/%.c=obj/%.o)
	$(CC) $(WARN) $^ -o bin/$@ $(LPTHREAD) $(INC)

obj/%.o: src/%.c
	$(CC) $(FLAGS) -c $< -o $@ $(INC) 

# Unitary Test
suiteRS232: unitaryTest/CU_rs232.c obj/rs232.o
	$(CC) $(CFLAGS) $^ -o bin/$@ $(CUNIT)

suiteRSGAMA: unitaryTest/CU_rsGama.c obj/rsGama.o
	$(CC) $(CFLAGS) $^ -o bin/$@ $(CUNIT)

prepare:
	mkdir -p obj/ bin/

# Clean all
clean:
	rm -f obj/*
	rm -f bin/*

