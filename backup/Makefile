all: main.o btree.o
	gcc -g main.o btree.o -o a
main.o: main.c btree.h
	gcc main.c -c
btree.o: btree.c bnode.h btree.h
	gcc btree.c -c
clean:
	rm *.o a
test:
	./a
