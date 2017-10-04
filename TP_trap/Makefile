CC = gcc
CFLAGS = -g -Wall -Werror
SRC = trap_fonction.c
OBJ = trap_fonction.o

trap_fonction : $(OBJ)
								$(CC) -o $@ $(OBJ)

trap_fonction.o : $(SRC)
									$(CC) $(CFLAGS) -c $<

clean :
			rm *.o trap_fonction
