# Makefile - kierke@bu.edu

#Declare:
COMP=gcc
FLAGS=-I.
#DEPS=fifo.h actors.h
OBJ=sim_main.o

# Constant code:
%.o: %.c
	$(COMP) -c -o $@ $< $(FLAGS)

# **Change filename**
simpleISS: $(OBJ)
	$(COMP) -o $@ $^ $(FLAGS)

clean:
	rm -f $(OBJ) simpleISS
