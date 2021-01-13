main:
	g++ -std=c++11 -Wall -o search.exe main.cpp MyAI.h MyAI.cpp
clean: 
	rm -rf *.exe