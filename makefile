CFLAGS=-std=c99 -Wall -pedantic -g
BIN=project
CC=gcc
RM=rm -f

ALL: scanner.o main.o navratova_hodnota.o precedencni_analyza.o syntakticka_analyza.o ilist.o ial.o mystring.o frame.o interpret.o
	$(CC) $(CFLAGS) -o $(BIN) scanner.o main.o navratova_hodnota.o precedencni_analyza.o syntakticka_analyza.o ilist.o ial.o mystring.o frame.o interpret.o

clean:
	$(RM) *.o $(BIN)
