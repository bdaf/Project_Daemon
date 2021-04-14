all: program
program: main.o  
	gcc main.o -o program
main.o: main.c
	gcc main.c -c
.PHONY: clean
clean:
	rm -f *.o program
