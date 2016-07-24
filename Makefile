SRC=main.cpp
SRC+=ChunkEngine.cpp
SRC+=MurmurHash2.cpp
SRC+=NearDuplicate.cpp
SRC+=ThreadedNearDuplicate.cpp

all:
	g++ -g -std=c++11 $(SRC) -L /usr/local/lib/ libpHash.a -lgomp -lpthread -o main

clean:
	rm -f main
