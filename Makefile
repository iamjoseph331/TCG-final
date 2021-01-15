main:
	g++ -std=c++11 -Wall -O3 -o search.exe main.cpp MyAI.h MyAI.cpp
debug:
	g++ -std=c++11 -Wall -g -o search.exe main.cpp MyAI.h MyAI.cpp
clean: 
	rm -rf *.exe *~ *.out