CXX=g++
FLAGS=-Wall -O3 -fmax-errors=3 -march=native -pipe
#FLAGS=-Wall -g -Og -std=c++17
SDL := $(shell sdl2-config --cflags)
LIBS=$(SDL)
FILES=$(filter-out src/main.cpp src/genTest.cpp src/mainMin.cpp, $(wildcard src/*.cpp))
ASOBJS=$(FILES:.cpp=.o)
FILESTEST=$(filter-out src/gen.cpp src/main.cpp src/mainMin.cpp, $(wildcard src/*.cpp))
ASOBJSTEST=$(FILESTEST:.cpp=.o)

all: gen run

genTest: $(ASOBJSTEST)
	$(CXX) $(FLAGS) -o genTest $(ASOBJSTEST)

gen: $(ASOBJS)
	$(CXX) $(FLAGS) -o gen $(ASOBJS)

run: src/main.cpp
	$(CXX) $(FLAGS) -o run src/main.cpp $(LIBS) -lSDL2 -fopenmp

runMin: src/mainMin.cpp
	$(CXX) $(FLAGS) -o runMin src/mainMin.cpp $(LIBS) -lSDL2 -fopenmp

%.o: %.cpp src/globals.h
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -f src/*.o run gen doom.qasm


