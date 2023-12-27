CC = gcc
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: fileorg

fileorg: $(OBJ)
	$(CC) -o $@ $^

$(OBJ): $(SRC)
	$(CC) -c $^

install: all
	cp fileorg $(HOME)/.local/bin
