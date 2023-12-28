CC 	= gcc
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: fileorg

$(OBJ): $(SRC)
	$(CC) -c $^

fileorg: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f fileorg $(OBJ)

install: all
	cp fileorg $(HOME)/.local/bin
