binspector: main.o
	gcc main.o -L /usr/lib -lbfd -L /usr/lib -lcurses -o binspector

main.o: main.c
	gcc -c main.c -L /usr/lib -llbfd -L /usr/lib -lcurses

clean:
	rm main.o binspector
