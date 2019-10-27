all: example
example: kvstore.o
	gcc kvstore.o main.c -o example
kvstore.o: kvstore.h
	gcc kvstore.c kvstore.h -c
clean:
	rm -rf kvstore.o kvstore.h.gch