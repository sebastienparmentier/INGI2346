#Variables de compilation
CC=gcc #Compilateur
CFLAGS=-Wall -Werror -ansi -pedantic -std=c99 -D_XOPEN_SOURCE=500 #options du compilateur
EXEC=myftp
SRC=$(wildcard src/*.c) 
OBJ=$(SRC:.c=.o) 
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

.PHONY: clean mproper

clean:
	rm -rf $(OBJ) *~

mproper: clean
	rm -f $(EXEC)

