main:
	g++ -std=c++11 -Wall -o search.exe main.cpp MyAI.h MyAI.cpp
debug:
	g++ -std=c++11 -g -o search.exe main.cpp MyAI.h MyAI.cpp
clean: 
	rm -rf *.exe