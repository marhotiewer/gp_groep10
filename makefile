server.out: main.o
	g++ main.o -o server.out -lwiringPi -lpthread

main.o: main.cpp
	g++ -Wno-psabi -c main.cpp

clean:
	rm -f *.o *.gch *.out
