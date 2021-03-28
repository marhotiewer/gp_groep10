server.out: main.o httplib.h.gch json.hpp.gch
	g++ main.o -o server.out -lwiringPi -lpthread

main.o: main.cpp
	g++ -Wno-psabi -c main.cpp

httplib.h.gch: httplib.h
	g++ -c httplib.h

json.hpp.gch: json.hpp
	g++ -c json.hpp
	
clean:
	rm -f *.o *.gch *.out