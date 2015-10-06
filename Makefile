# Makefile de exemplo

par_shell: commandlinereader.o list.o main.o fibonacci.o
	gcc -o par_shell commandlinereader.o list.o main.o
	gcc -o fibonacci fibonacci.o

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc -Wall -g -c commandlinereader.c

list.o: list.c list.h
	gcc -Wall -g -c list.c

fibonacci.o: fibonacci.c
	gcc -Wall -g -c fibonacci.c

main.o: main.c commandlinereader.h list.h
	gcc -Wall -g -c main.c

clean:
	rm -f *.o core par_shell fibonacci && clear

rebuild:
	rm -f .o* core par_shell fibonacci && clear && make && ./par_shell