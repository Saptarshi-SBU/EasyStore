SRC=main.cpp
SRC+=ChunkEngine.cpp
SRC+=MurmurHash2.cpp
SRC+=NearDuplicate.cpp
SRC+=ThreadedNearDuplicate.cpp
SRC+=ImageScale.cpp
SRC+=ThreadedImageScale.cpp

all:
	g++ -g -std=c++11 $(SRC) -I /usr/local/include/opencv -L /usr/local/lib -lm -lopencv_core -lopencv_imgproc -lopencv_highgui -L /usr/local/lib/ libpHash.a -lgomp -lpthread -o main

clean:
	rm -f main
