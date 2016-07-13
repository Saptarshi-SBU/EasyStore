SRC=main.cpp
SRC+=ChunkEngine.cpp
SRC+=MurmurHash2.cpp

all:
	g++ -o3 -std=c++11 $(SRC) -o main
	#g++ -g -DDEBUG -std=c++11 $(SRC) -o main

clean:
	rm -f main
