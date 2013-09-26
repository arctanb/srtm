CC=gcc
CFLAGS=-std=c99 -O4 -lm -g -Wall

SRC=main.c

a.out: $(SRC)
	$(CC) -o a.out $(SRC) $(CFLAGS)

clean:
	rm -f a.out
