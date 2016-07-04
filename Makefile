SRC=main.cpp
SRC+=ChunkEngine.cpp
SRC+=MurmurHash2.cpp

all:
	g++ -g -DDEBUG -std=c++11 $(SRC) -o main

clean:
	rm -f main
