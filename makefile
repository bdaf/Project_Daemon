OBJ = main.o read_files.o
all: program
program: $(OBJ) 
	gcc $(OBJ) -o program
main.o: main.c
	gcc main.c -c
read_files.o: read_files.c read_files.h
	gcc read_files.c -c
.PHONY: clean
clean:
	rm -f *.o program
