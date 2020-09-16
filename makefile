all: dm.o
	gcc dm.o -o Primalite -lgmp

dm.o: dm.c 
	gcc -o dm.o -c dm.c -lgmp 

run:
	./Primalite

clean:
	rm -rf Primalite *.o
